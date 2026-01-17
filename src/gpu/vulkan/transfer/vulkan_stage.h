#ifndef GPU_VULKAN_TRANSFER_STAGE_H_
#define GPU_VULKAN_TRANSFER_STAGE_H_

#include <optional>

#include <volk.h>

#include "gpu/vulkan/builder/vulkan_resource.h"

std::optional<VulkanBufferResource> createStagingBuffer(VkPhysicalDevice physical_device, VkDevice device, VkDeviceSize buffer_size);

void copyBufferToImage(VkCommandBuffer command_buffer, VkBuffer src_buffer, VkImage dst_image, VkExtent3D extent, VkImageSubresourceLayers subresource_layers = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1});

void copyImageToBuffer(VkCommandBuffer command_buffer, VkImage src_image, VkBuffer dst_buffer, VkExtent3D extent, VkImageSubresourceLayers subresource_layers = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1});

void transitionImageLayout(VkCommandBuffer command_buffer, VkImage image, VkImageLayout old_layout, VkImageLayout new_layout, VkImageAspectFlags aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT);

#endif /* GPU_VULKAN_TRANSFER_STAGE_H_ */
