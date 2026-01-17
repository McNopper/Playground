#ifndef GPU_VULKAN_PRESENTATION_IVULKANWINDOW_H_
#define GPU_VULKAN_PRESENTATION_IVULKANWINDOW_H_

#include "core/math/vector.h"

#include <volk.h>

class IVulkanWindow
{

public:

	virtual ~IVulkanWindow() = default;

	virtual VkExtent2D getCurrentExtent() const = 0;

	virtual VkSurfaceFormatKHR getSurfaceFormat() const = 0;

	virtual VkFormat getDepthStencilFormat() const = 0;

	virtual VkSampleCountFlagBits getSamples() const = 0;

	virtual void setClearColor(float red, float green, float blue, float alpha) = 0;

	virtual void setClearColor(const float4& clear_color) = 0;

	virtual void setClearDepth(float depth) = 0;

	virtual void setClearStencil(uint32_t stencil) = 0;

	virtual void beginRendering() const = 0;

	virtual void endRendering() const = 0;

};

#endif /* GPU_VULKAN_PRESENTATION_IVULKANWINDOW_H_ */
