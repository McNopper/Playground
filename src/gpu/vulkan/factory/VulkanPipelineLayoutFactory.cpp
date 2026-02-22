#include "VulkanPipelineLayoutFactory.h"

VulkanPipelineLayoutFactory::VulkanPipelineLayoutFactory(VkDevice device, VkPipelineLayoutCreateFlags flags) :
    m_device{ device },
    m_flags{ flags }
{
}

void VulkanPipelineLayoutFactory::addDescriptorSetLayout(VkDescriptorSetLayout descriptor_set_layout)
{
    m_descriptor_set_layouts.push_back(descriptor_set_layout);
}

void VulkanPipelineLayoutFactory::addPushConstantRange(const VkPushConstantRange& push_constant_range)
{
    m_push_constant_ranges.push_back(push_constant_range);
}

VkPipelineLayout VulkanPipelineLayoutFactory::create() const
{
    VkPipelineLayoutCreateInfo pipeline_layout_create_info{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    pipeline_layout_create_info.flags = m_flags;
    pipeline_layout_create_info.setLayoutCount = (uint32_t)m_descriptor_set_layouts.size();
    if (!m_descriptor_set_layouts.empty())
    {
        pipeline_layout_create_info.pSetLayouts = m_descriptor_set_layouts.data();
    }
    pipeline_layout_create_info.pushConstantRangeCount = (uint32_t)m_push_constant_ranges.size();
    if (!m_push_constant_ranges.empty())
    {
        pipeline_layout_create_info.pPushConstantRanges = m_push_constant_ranges.data();
    }

    VkPipelineLayout pipeline_layout{ VK_NULL_HANDLE };
    vkCreatePipelineLayout(m_device, &pipeline_layout_create_info, nullptr, &pipeline_layout);

    return pipeline_layout;
}
