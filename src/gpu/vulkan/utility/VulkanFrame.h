#ifndef GPU_VULKAN_UTILITY_VULKANFRAME_H_
#define GPU_VULKAN_UTILITY_VULKANFRAME_H_

#include <cstdint>

#include <volk.h>

#include "core/math/vector.h"

class VulkanFrame
{

private:

	VkImageView m_color_image_view{ VK_NULL_HANDLE };
	VkImageView m_msaa_image_view{ VK_NULL_HANDLE };
	VkImageView m_depth_image_view{ VK_NULL_HANDLE };
	VkImageView m_stencil_image_view{ VK_NULL_HANDLE };

	float4 m_clear_color{0.0f, 0.0f, 0.0f, 1.0};
	float m_clear_depth{ 1.0f };
	uint32_t m_clear_stencil{ 0u };

	VkRect2D m_render_area{ {0, 0}, {0u, 0u} };

public:

	VulkanFrame(VkImageView color_image_view, const VkRect2D& render_area);

	void setMsaaImageView(VkImageView image_view);

	void setDepthImageView(VkImageView image_view);

	void setStencilImageView(VkImageView image_view);

	void setClearColor(float red, float green, float blue, float alpha);

	void setClearColor(const float4& clear_color);

	void setClearDepth(float depth);

	void setClearStencil(uint32_t stencil);

	void beginRendering(VkCommandBuffer command_buffer) const;

};

#endif /* GPU_VULKAN_UTILITY_VULKANFRAME_H_ */
