#ifndef GPU_VULKAN_FACTORY_VULKANPIPELINELAYOUTFACTORY_H_
#define GPU_VULKAN_FACTORY_VULKANPIPELINELAYOUTFACTORY_H_

#include <vector>

#include <volk.h>

class VulkanPipelineLayoutFactory
{

private:

    VkDevice m_device{ VK_NULL_HANDLE };
    VkPipelineLayoutCreateFlags m_flags{};

    std::vector<VkDescriptorSetLayout> m_descriptor_set_layouts{};
    std::vector<VkPushConstantRange> m_push_constant_ranges{};

public:

    VulkanPipelineLayoutFactory() = delete;

    VulkanPipelineLayoutFactory(VkDevice device, VkPipelineLayoutCreateFlags flags);

    void addDescriptorSetLayout(VkDescriptorSetLayout descriptor_set_layout);

    void addPushConstantRange(const VkPushConstantRange& push_constant_range);

    VkPipelineLayout create() const;

};

#endif /* GPU_VULKAN_FACTORY_VULKANPIPELINELAYOUTFACTORY_H_ */
