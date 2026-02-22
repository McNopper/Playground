#ifndef GPU_VULKAN_FACTORY_VULKANSEMAPHOREFACTORY_H_
#define GPU_VULKAN_FACTORY_VULKANSEMAPHOREFACTORY_H_

#include <volk.h>

class VulkanSemaphoreFactory
{

private:

    VkDevice m_device{ VK_NULL_HANDLE };

    VkSemaphoreCreateFlags m_semaphore_create_flags{ 0u };

public:

    VulkanSemaphoreFactory() = delete;

    VulkanSemaphoreFactory(VkDevice device, VkSemaphoreCreateFlags semaphore_create_flags);

    VkSemaphore create() const;

    VkSemaphore createTimeline(uint64_t initial_value) const;

};

#endif /* GPU_VULKAN_FACTORY_VULKANSEMAPHOREFACTORY_H_ */
