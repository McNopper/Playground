#ifndef GPU_VULKAN_UTILITY_VULKAN_HELPER_H_
#define GPU_VULKAN_UTILITY_VULKAN_HELPER_H_

#include <volk.h>

#include "core/color/types.h"

struct ImageData;

// Format and color

VkFormat getVulkanFormat(const ImageData& image_data);

uint32_t getFormatSize(VkFormat format);

bool isFormatDepthOnly(VkFormat format);

bool isFormatStencilOnly(VkFormat format);

bool isFormatDepthStencilOnly(VkFormat format);

bool isFormatDepthStencil(VkFormat format);

bool isFormatSRGB(VkFormat format);

bool isColorSpaceLinear(VkColorSpaceKHR color_space);

ColorSpace getColorSpace(VkColorSpaceKHR color_space);

TransferFunction getTransferFunction(VkColorSpaceKHR color_space);

TransferFunction getTransferFunction(VkSurfaceFormatKHR surface_format);

#endif /* GPU_VULKAN_UTILITY_VULKAN_HELPER_H_ */
