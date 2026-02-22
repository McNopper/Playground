#ifndef GPU_VULKAN_FACTORY_VULKANCOMMANDBUFFERFACTORY_H_
#define GPU_VULKAN_FACTORY_VULKANCOMMANDBUFFERFACTORY_H_

#include <vector>

#include <volk.h>

class VulkanCommandBufferFactory
{

private:

    VkDevice m_device{ VK_NULL_HANDLE };

    VkCommandPool m_command_pool{ VK_NULL_HANDLE };
    VkCommandBufferLevel m_command_buffer_level{ VK_COMMAND_BUFFER_LEVEL_PRIMARY };
    uint32_t m_command_buffer_count{ 0u };

public:

    VulkanCommandBufferFactory() = delete;

    VulkanCommandBufferFactory(VkDevice device, VkCommandPool command_pool, VkCommandBufferLevel command_buffer_level, uint32_t command_buffer_count);

    std::vector<VkCommandBuffer> create() const;

};

#endif /* GPU_VULKAN_FACTORY_VULKANCOMMANDBUFFERFACTORY_H_ */
