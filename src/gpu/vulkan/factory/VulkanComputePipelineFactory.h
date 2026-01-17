#ifndef GPU_VULKAN_FACTORY_VULKANCOMPUTEPIPELINEFACTORY_H_
#define GPU_VULKAN_FACTORY_VULKANCOMPUTEPIPELINEFACTORY_H_

#include <volk.h>

class VulkanComputePipelineFactory
{

private:

	VkDevice m_device{ VK_NULL_HANDLE };

	VkPipelineCache m_pipeline_cache{ VK_NULL_HANDLE };

	VkPipelineCreateFlags m_flags{ 0u };
	VkPipelineShaderStageCreateInfo m_stage{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
	VkPipelineLayout m_layout{ VK_NULL_HANDLE };
	VkPipeline m_base_pipeline_handle{ VK_NULL_HANDLE };
	int32_t m_base_pipeline_index{ -1 };

public:

	VulkanComputePipelineFactory() = delete;

	VulkanComputePipelineFactory(VkDevice device, VkPipelineCreateFlags flags, const VkPipelineShaderStageCreateInfo& stage, VkPipelineLayout layout);

	VkPipeline create() const;

};

#endif /* GPU_VULKAN_FACTORY_VULKANCOMPUTEPIPELINEFACTORY_H_ */
