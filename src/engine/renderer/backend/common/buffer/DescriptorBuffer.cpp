#include "DescriptorBuffer.h"

#include "gpu/gpu.h"

DescriptorBuffer::DescriptorBuffer(VkPhysicalDevice physical_device, VkDevice device) :
	GpuBuffer(physical_device, device)
{
}

DescriptorBuffer::~DescriptorBuffer()
{
}

bool DescriptorBuffer::queryDescriptorSize()
{
	auto descriptor_buffer_properties = gatherPhysicalDeviceDescriptorBufferPropertiesEXT(m_physical_device);

	switch (m_descriptor_type)
	{
		case VK_DESCRIPTOR_TYPE_SAMPLER:
			m_descriptor_size = descriptor_buffer_properties.samplerDescriptorSize;
			break;
		case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
			m_descriptor_size = descriptor_buffer_properties.combinedImageSamplerDescriptorSize;
			break;
		case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
			m_descriptor_size = descriptor_buffer_properties.sampledImageDescriptorSize;
			break;
		case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
			m_descriptor_size = descriptor_buffer_properties.storageImageDescriptorSize;
			break;
		case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
			m_descriptor_size = descriptor_buffer_properties.uniformTexelBufferDescriptorSize;
			break;
		case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
			m_descriptor_size = descriptor_buffer_properties.storageTexelBufferDescriptorSize;
			break;
		case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
			m_descriptor_size = descriptor_buffer_properties.uniformBufferDescriptorSize;
			break;
		case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
			m_descriptor_size = descriptor_buffer_properties.storageBufferDescriptorSize;
			break;
		case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
			m_descriptor_size = descriptor_buffer_properties.inputAttachmentDescriptorSize;
			break;
		case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
			m_descriptor_size = descriptor_buffer_properties.accelerationStructureDescriptorSize;
			break;
		default:
			return false;
	}

	return m_descriptor_size > 0u;
}

bool DescriptorBuffer::create(VkDeviceSize descriptor_count, VkDescriptorType descriptor_type)
{
	m_descriptor_type = descriptor_type;

	if (!queryDescriptorSize())
	{
		return false;
	}

	VkDeviceSize total_size = descriptor_count * m_descriptor_size;

	VkBufferUsageFlags usage = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

	switch (descriptor_type)
	{
		case VK_DESCRIPTOR_TYPE_SAMPLER:
		case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
		case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
		case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
		case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
			usage |= VK_BUFFER_USAGE_SAMPLER_DESCRIPTOR_BUFFER_BIT_EXT;
			break;
		case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
		case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
		case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
		case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
		case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
			usage |= VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT;
			break;
		default:
			return false;
	}

	if (!GpuBuffer::create(total_size, usage))
	{
		return false;
	}

	VkBufferDeviceAddressInfo address_info{ VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO };
	address_info.buffer = m_buffer_resource.buffer;

	m_device_address = vkGetBufferDeviceAddress(m_device, &address_info);

	return m_device_address != 0u;
}

bool DescriptorBuffer::writeImageDescriptor(VkDeviceSize index, const VkDescriptorImageInfo& image_info)
{
	if (!isValid())
	{
		return false;
	}

	if (m_descriptor_type != VK_DESCRIPTOR_TYPE_SAMPLER &&
	    m_descriptor_type != VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER &&
	    m_descriptor_type != VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE &&
	    m_descriptor_type != VK_DESCRIPTOR_TYPE_STORAGE_IMAGE &&
	    m_descriptor_type != VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
	{
		return false;
	}

	VkDeviceSize offset = index * m_descriptor_size;

	VkDescriptorGetInfoEXT descriptor_get_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT };
	descriptor_get_info.type = m_descriptor_type;
	descriptor_get_info.data.pCombinedImageSampler = &image_info;

	std::vector<std::uint8_t> descriptor_data(m_descriptor_size);

	vkGetDescriptorEXT(m_device, &descriptor_get_info, m_descriptor_size, descriptor_data.data());

	return update(offset, descriptor_data);
}

bool DescriptorBuffer::writeBufferDescriptor(VkDeviceSize index, const VkDescriptorBufferInfo& buffer_info)
{
	if (!isValid())
	{
		return false;
	}

	if (m_descriptor_type != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER &&
	    m_descriptor_type != VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
	{
		return false;
	}

	VkDeviceSize offset = index * m_descriptor_size;

	// Get the device address of the buffer.
	// IMPORTANT: buffer_info.buffer MUST have been created with VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT.
	// For uniform buffers used with descriptor buffers, create them with:
	// GpuBuffer::create(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
	VkBufferDeviceAddressInfo buffer_address_info{ VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO };
	buffer_address_info.buffer = buffer_info.buffer;
	VkDeviceAddress buffer_address = vkGetBufferDeviceAddress(m_device, &buffer_address_info);

	// Create descriptor address info with the buffer's device address
	VkDescriptorAddressInfoEXT address_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_ADDRESS_INFO_EXT };
	address_info.address = buffer_address + buffer_info.offset;
	address_info.range = buffer_info.range;
	address_info.format = VK_FORMAT_UNDEFINED;

	// Get the descriptor data using VK_EXT_descriptor_buffer
	VkDescriptorGetInfoEXT descriptor_get_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT };
	descriptor_get_info.type = m_descriptor_type;
	descriptor_get_info.data.pUniformBuffer = &address_info;

	std::vector<std::uint8_t> descriptor_data(m_descriptor_size);
	vkGetDescriptorEXT(m_device, &descriptor_get_info, m_descriptor_size, descriptor_data.data());

	// Write the descriptor data to the descriptor buffer
	return update(offset, descriptor_data);
}

bool DescriptorBuffer::writeAccelerationStructureDescriptor(VkDeviceSize index, VkAccelerationStructureKHR acceleration_structure)
{
	if (!isValid())
	{
		return false;
	}

	if (m_descriptor_type != VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR)
	{
		return false;
	}

	VkDeviceSize offset = index * m_descriptor_size;

	// Get the device address of the acceleration structure
	VkAccelerationStructureDeviceAddressInfoKHR as_address_info{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR };
	as_address_info.accelerationStructure = acceleration_structure;
	VkDeviceAddress as_address = vkGetAccelerationStructureDeviceAddressKHR(m_device, &as_address_info);

	// Get the descriptor data using VK_EXT_descriptor_buffer
	VkDescriptorGetInfoEXT descriptor_get_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT };
	descriptor_get_info.type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	descriptor_get_info.data.accelerationStructure = as_address;

	std::vector<std::uint8_t> descriptor_data(m_descriptor_size);
	vkGetDescriptorEXT(m_device, &descriptor_get_info, m_descriptor_size, descriptor_data.data());

	// Write the descriptor data to the descriptor buffer
	return update(offset, descriptor_data);
}

VkDeviceAddress DescriptorBuffer::getDeviceAddress() const
{
	return m_device_address;
}

VkDeviceSize DescriptorBuffer::getDescriptorSize() const
{
	return m_descriptor_size;
}

VkDescriptorType DescriptorBuffer::getDescriptorType() const
{
	return m_descriptor_type;
}
