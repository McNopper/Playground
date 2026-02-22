#ifndef GPU_VULKAN_TRANSFER_VULKAN_STAGE_H_
#define GPU_VULKAN_TRANSFER_VULKAN_STAGE_H_

#include <optional>

#include <volk.h>

#include "gpu/vulkan/builder/vulkan_resource.h"

std::optional<VulkanBufferResource> createStagingBuffer(VkPhysicalDevice physical_device, VkDevice device, VkDeviceSize buffer_size);

void copyBufferToImage(VkCommandBuffer command_buffer, VkBuffer src_buffer, VkImage dst_image, VkExtent3D extent, VkImageSubresourceLayers subresource_layers = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 });

void copyImageToBuffer(VkCommandBuffer command_buffer, VkImage src_image, VkBuffer dst_buffer, VkExtent3D extent, VkImageSubresourceLayers subresource_layers = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 });

void transitionImageLayout(VkCommandBuffer command_buffer, VkImage image, VkImageLayout old_layout, VkImageLayout new_layout, VkImageAspectFlags aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT, uint32_t base_mip_level = 0u, uint32_t level_count = 1u, uint32_t layer_count = 1u);

void copyHostToImage(VkDevice device, const void* src_data, uint32_t src_row_length, uint32_t src_image_height, VkImage dst_image, VkImageLayout dst_image_layout, VkExtent3D extent, VkImageSubresourceLayers subresource_layers = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 });

#endif /* GPU_VULKAN_TRANSFER_VULKAN_STAGE_H_ */
