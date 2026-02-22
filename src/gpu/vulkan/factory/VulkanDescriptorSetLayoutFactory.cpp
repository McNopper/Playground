#include "VulkanDescriptorSetLayoutFactory.h"

VulkanDescriptorSetLayoutFactory::VulkanDescriptorSetLayoutFactory(VkDevice device, VkDescriptorSetLayoutCreateFlags flags) :
    m_device{ device },
    m_flags{ flags }
{
}

void VulkanDescriptorSetLayoutFactory::addDescriptorSetLayoutBinding(const VkDescriptorSetLayoutBinding& descriptor_set_layout_binding)
{
    m_descriptor_set_layout_bindings.push_back(descriptor_set_layout_binding);
}

VkDescriptorSetLayout VulkanDescriptorSetLayoutFactory::create() const
{
    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    descriptor_set_layout_create_info.flags = m_flags;
    descriptor_set_layout_create_info.bindingCount = (uint32_t)m_descriptor_set_layout_bindings.size();
    if (!m_descriptor_set_layout_bindings.empty())
    {
        descriptor_set_layout_create_info.pBindings = m_descriptor_set_layout_bindings.data();
    }

    VkDescriptorSetLayout descriptor_set_layout{ VK_NULL_HANDLE };
    vkCreateDescriptorSetLayout(m_device, &descriptor_set_layout_create_info, nullptr, &descriptor_set_layout);

    return descriptor_set_layout;
}
