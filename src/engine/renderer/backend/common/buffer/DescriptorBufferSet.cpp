#include "DescriptorBufferSet.h"

#include "gpu/gpu.h"

DescriptorBufferSet::DescriptorBufferSet(VkPhysicalDevice physical_device, VkDevice device) :
    GpuBuffer(physical_device, device)
{
}

DescriptorBufferSet::~DescriptorBufferSet()
{
}

bool DescriptorBufferSet::queryDescriptorSizes()
{
    auto descriptor_buffer_properties = gatherPhysicalDeviceDescriptorBufferPropertiesEXT(m_physical_device);
    m_alignment = descriptor_buffer_properties.descriptorBufferOffsetAlignment;

    // Query and store sizes for each binding
    for (auto& [binding_index, binding] : m_bindings)
    {
        binding.size = getDescriptorSize(binding.type);
        if (binding.size == 0u)
        {
            return false;
        }
    }

    return true;
}

VkDeviceSize DescriptorBufferSet::getDescriptorSize(VkDescriptorType type) const
{
    auto descriptor_buffer_properties = gatherPhysicalDeviceDescriptorBufferPropertiesEXT(m_physical_device);

    switch (type)
    {
        case VK_DESCRIPTOR_TYPE_SAMPLER:
            return descriptor_buffer_properties.samplerDescriptorSize;
        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            return descriptor_buffer_properties.combinedImageSamplerDescriptorSize;
        case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            return descriptor_buffer_properties.sampledImageDescriptorSize;
        case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            return descriptor_buffer_properties.storageImageDescriptorSize;
        case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
            return descriptor_buffer_properties.uniformTexelBufferDescriptorSize;
        case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
            return descriptor_buffer_properties.storageTexelBufferDescriptorSize;
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            return descriptor_buffer_properties.uniformBufferDescriptorSize;
        case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            return descriptor_buffer_properties.storageBufferDescriptorSize;
        case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
            return descriptor_buffer_properties.inputAttachmentDescriptorSize;
        case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
            return descriptor_buffer_properties.accelerationStructureDescriptorSize;
        default:
            return 0u;
    }
}

VkDeviceSize DescriptorBufferSet::alignOffset(VkDeviceSize offset) const
{
    if (m_alignment == 0u)
    {
        return offset;
    }

    return ((offset + m_alignment - 1) / m_alignment) * m_alignment;
}

bool DescriptorBufferSet::addBinding(uint32_t binding, VkDescriptorType type)
{
    if (m_bindings.contains(binding))
    {
        return false; // Binding already exists
    }

    DescriptorBinding desc_binding{};
    desc_binding.binding = binding;
    desc_binding.type = type;

    m_bindings[binding] = desc_binding;
    return true;
}

bool DescriptorBufferSet::create()
{
    if (m_bindings.empty())
    {
        return false;
    }

    if (!queryDescriptorSizes())
    {
        return false;
    }

    // Calculate total size - descriptors are packed sequentially (no alignment between them)
    VkDeviceSize total_size = 0u;
    for (auto& [binding_index, binding] : m_bindings)
    {
        binding.offset = total_size; // No alignment between descriptors
        total_size = binding.offset + binding.size;
    }

    // Determine usage flags based on descriptor types
    VkBufferUsageFlags usage = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    bool has_resource_descriptors = false;
    bool has_sampler_descriptors = false;

    for (const auto& [binding_index, binding] : m_bindings)
    {
        switch (binding.type)
        {
            case VK_DESCRIPTOR_TYPE_SAMPLER:
            case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
                has_sampler_descriptors = true;
                break;
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
            case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
            case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
                has_resource_descriptors = true;
                break;
            default:
                return false;
        }
    }

    if (has_resource_descriptors)
    {
        usage |= VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT;
    }
    if (has_sampler_descriptors)
    {
        usage |= VK_BUFFER_USAGE_SAMPLER_DESCRIPTOR_BUFFER_BIT_EXT;
    }

    m_usage = usage; // Store usage flags

    if (!GpuBuffer::create(total_size, usage))
    {
        return false;
    }

    VkBufferDeviceAddressInfo address_info{ VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO };
    address_info.buffer = m_buffer_resource.buffer;

    m_device_address = vkGetBufferDeviceAddress(m_device, &address_info);

    return m_device_address != 0u;
}

bool DescriptorBufferSet::writeImageDescriptor(uint32_t binding, const VkDescriptorImageInfo& image_info)
{
    if (!isValid())
    {
        return false;
    }

    auto it = m_bindings.find(binding);
    if (it == m_bindings.end())
    {
        return false;
    }

    const DescriptorBinding& desc_binding = it->second;

    if (desc_binding.type != VK_DESCRIPTOR_TYPE_SAMPLER &&
        desc_binding.type != VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER &&
        desc_binding.type != VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE &&
        desc_binding.type != VK_DESCRIPTOR_TYPE_STORAGE_IMAGE &&
        desc_binding.type != VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
    {
        return false;
    }

    VkDescriptorGetInfoEXT descriptor_get_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT };
    descriptor_get_info.type = desc_binding.type;
    descriptor_get_info.data.pCombinedImageSampler = &image_info;

    std::vector<std::uint8_t> descriptor_data(desc_binding.size);
    vkGetDescriptorEXT(m_device, &descriptor_get_info, desc_binding.size, descriptor_data.data());

    return update(desc_binding.offset, descriptor_data);
}

bool DescriptorBufferSet::writeBufferDescriptor(uint32_t binding, const VkDescriptorBufferInfo& buffer_info)
{
    if (!isValid())
    {
        return false;
    }

    auto it = m_bindings.find(binding);
    if (it == m_bindings.end())
    {
        return false;
    }

    const DescriptorBinding& desc_binding = it->second;

    if (desc_binding.type != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER &&
        desc_binding.type != VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
    {
        return false;
    }

    VkBufferDeviceAddressInfo buffer_address_info{ VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO };
    buffer_address_info.buffer = buffer_info.buffer;
    VkDeviceAddress buffer_address = vkGetBufferDeviceAddress(m_device, &buffer_address_info);

    VkDescriptorAddressInfoEXT address_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_ADDRESS_INFO_EXT };
    address_info.address = buffer_address + buffer_info.offset;
    address_info.range = buffer_info.range;
    address_info.format = VK_FORMAT_UNDEFINED;

    VkDescriptorGetInfoEXT descriptor_get_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT };
    descriptor_get_info.type = desc_binding.type;
    descriptor_get_info.data.pUniformBuffer = &address_info;

    std::vector<std::uint8_t> descriptor_data(desc_binding.size);
    vkGetDescriptorEXT(m_device, &descriptor_get_info, desc_binding.size, descriptor_data.data());

    return update(desc_binding.offset, descriptor_data);
}

bool DescriptorBufferSet::writeAccelerationStructureDescriptor(uint32_t binding, VkAccelerationStructureKHR acceleration_structure)
{
    if (!isValid())
    {
        return false;
    }

    auto it = m_bindings.find(binding);
    if (it == m_bindings.end())
    {
        return false;
    }

    const DescriptorBinding& desc_binding = it->second;

    if (desc_binding.type != VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR)
    {
        return false;
    }

    VkAccelerationStructureDeviceAddressInfoKHR as_address_info{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR };
    as_address_info.accelerationStructure = acceleration_structure;
    VkDeviceAddress as_address = vkGetAccelerationStructureDeviceAddressKHR(m_device, &as_address_info);

    VkDescriptorGetInfoEXT descriptor_get_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT };
    descriptor_get_info.type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    descriptor_get_info.data.accelerationStructure = as_address;

    std::vector<std::uint8_t> descriptor_data(desc_binding.size);
    vkGetDescriptorEXT(m_device, &descriptor_get_info, desc_binding.size, descriptor_data.data());

    return update(desc_binding.offset, descriptor_data);
}

VkDeviceSize DescriptorBufferSet::getBindingOffset(uint32_t binding) const
{
    auto it = m_bindings.find(binding);
    if (it == m_bindings.end())
    {
        return 0u;
    }
    return it->second.offset;
}

VkDeviceAddress DescriptorBufferSet::getDeviceAddress() const
{
    return m_device_address;
}

VkBufferUsageFlags DescriptorBufferSet::getUsageFlags() const
{
    return m_usage;
}
