#ifndef GPU_VULKAN_FACTORY_VULKANFENCEFACTORY_H_
#define GPU_VULKAN_FACTORY_VULKANFENCEFACTORY_H_

#include <volk.h>

class VulkanFenceFactory
{

private:

    VkDevice m_device{ VK_NULL_HANDLE };

    VkFenceCreateFlags m_fence_create_flags{ 0u };

public:

    VulkanFenceFactory() = delete;

    VulkanFenceFactory(VkDevice device, VkFenceCreateFlags fence_create_flags);

    VkFence create() const;

};

#endif /* GPU_VULKAN_FACTORY_VULKANFENCEFACTORY_H_ */
