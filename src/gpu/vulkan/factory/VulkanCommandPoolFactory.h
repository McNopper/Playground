#ifndef GPU_VULKAN_FACTORY_VULKANCOMMANDPOOLFACTORY_H_
#define GPU_VULKAN_FACTORY_VULKANCOMMANDPOOLFACTORY_H_

#include <cstdint>

#include <volk.h>

class VulkanCommandPoolFactory
{

private:

	VkDevice m_device{ VK_NULL_HANDLE };
	VkCommandPoolCreateFlags m_command_pool_create_flags{ 0u };
	uint32_t m_queue_family_index{ 0u };

public:

	VulkanCommandPoolFactory() = delete;

	VulkanCommandPoolFactory(VkDevice device, VkCommandPoolCreateFlags command_pool_create_flags, uint32_t queue_family_index);

	VkCommandPool create() const;

};

#endif /* GPU_VULKAN_FACTORY_VULKANCOMMANDPOOLFACTORY_H_ */
