#include "vulkan_resource.h"

void destroyResource(VkDevice device, VulkanImageResource& image_resource)
{
	if (image_resource.image_view != VK_NULL_HANDLE)
	{
		vkDestroyImageView(device, image_resource.image_view, nullptr);
		image_resource.image_view = VK_NULL_HANDLE;
	}

	if (image_resource.device_memory != VK_NULL_HANDLE)
	{
		vkFreeMemory(device, image_resource.device_memory, nullptr);
		image_resource.device_memory = VK_NULL_HANDLE;
	}

	if (image_resource.image != VK_NULL_HANDLE)
	{
		vkDestroyImage(device, image_resource.image, nullptr);
		image_resource.image = VK_NULL_HANDLE;
	}
}

void destroyResource(VkDevice device, VulkanBufferResource& buffer_resource)
{
	if (buffer_resource.device_memory != VK_NULL_HANDLE)
	{
		vkFreeMemory(device, buffer_resource.device_memory, nullptr);
		buffer_resource.device_memory = VK_NULL_HANDLE;
	}

	if (buffer_resource.buffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(device, buffer_resource.buffer, nullptr);
		buffer_resource.buffer = VK_NULL_HANDLE;
	}
}
