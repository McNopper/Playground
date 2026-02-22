#ifndef GPU_VULKAN_BUILDER_VULKANDEVICEMEMORY_H_
#define GPU_VULKAN_BUILDER_VULKANDEVICEMEMORY_H_

#include <volk.h>

VkDeviceMemory buildImageDeviceMemory(VkPhysicalDevice physical_device, VkDevice device, VkImage image, VkMemoryPropertyFlags memory_property_flags);

VkDeviceMemory buildBufferDeviceMemory(VkPhysicalDevice physical_device, VkDevice device, VkBuffer buffer, VkMemoryPropertyFlags memory_property_flags, VkMemoryAllocateFlags memory_allocate_flags = 0u);

#endif /* GPU_VULKAN_BUILDER_VULKANDEVICEMEMORY_H_ */
