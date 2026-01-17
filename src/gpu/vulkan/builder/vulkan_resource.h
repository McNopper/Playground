#ifndef GPU_VULKAN_BUILDER_VULKANRESOURCE_H_
#define GPU_VULKAN_BUILDER_VULKANRESOURCE_H_

#include <volk.h>

struct VulkanImageResource {
	VkImage image{ VK_NULL_HANDLE };
	VkDeviceMemory device_memory{ VK_NULL_HANDLE };
	VkImageView image_view{ VK_NULL_HANDLE };
};

struct VulkanBufferResource {
	VkBuffer buffer{ VK_NULL_HANDLE };
	VkDeviceMemory device_memory{ VK_NULL_HANDLE };
};

void destroyResource(VkDevice device, VulkanImageResource& image_resource);

void destroyResource(VkDevice device, VulkanBufferResource& buffer_resource);

#endif /* GPU_VULKAN_BUILDER_VULKANRESOURCE_H_ */