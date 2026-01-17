#include "VulkanComputePipelineFactory.h"

VulkanComputePipelineFactory::VulkanComputePipelineFactory(VkDevice device, VkPipelineCreateFlags flags, const VkPipelineShaderStageCreateInfo& stage, VkPipelineLayout layout) :
	m_device{ device }, m_flags{ flags }, m_stage{ stage }, m_layout{ layout }
{
}

VkPipeline VulkanComputePipelineFactory::create() const
{
	VkComputePipelineCreateInfo compute_pipeline_create_info{ VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };
	compute_pipeline_create_info.flags = m_flags;
	compute_pipeline_create_info.stage = m_stage;
	compute_pipeline_create_info.layout = m_layout;
	compute_pipeline_create_info.basePipelineHandle = m_base_pipeline_handle;
	compute_pipeline_create_info.basePipelineIndex = m_base_pipeline_index;

	VkPipeline pipeline{ VK_NULL_HANDLE };
	vkCreateComputePipelines(m_device, m_pipeline_cache, 1u, &compute_pipeline_create_info, nullptr, &pipeline);

	return pipeline;
}
