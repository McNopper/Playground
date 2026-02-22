#ifndef GPU_VULKAN_FACTORY_VULKANDESCRIPTORSETLAYOUTFACTORY_H_
#define GPU_VULKAN_FACTORY_VULKANDESCRIPTORSETLAYOUTFACTORY_H_

#include <vector>

#include <volk.h>

class VulkanDescriptorSetLayoutFactory
{

private:

    VkDevice m_device{ VK_NULL_HANDLE };
    VkDescriptorSetLayoutCreateFlags m_flags{};

    std::vector<VkDescriptorSetLayoutBinding> m_descriptor_set_layout_bindings{};

public:

    VulkanDescriptorSetLayoutFactory() = delete;

    VulkanDescriptorSetLayoutFactory(VkDevice device, VkDescriptorSetLayoutCreateFlags flags);

    void addDescriptorSetLayoutBinding(const VkDescriptorSetLayoutBinding& descriptor_set_layout_binding);

    VkDescriptorSetLayout create() const;
};

#endif /* GPU_VULKAN_FACTORY_VULKANDESCRIPTORSETLAYOUTFACTORY_H_ */
