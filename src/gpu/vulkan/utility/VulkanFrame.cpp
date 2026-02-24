#include "VulkanFrame.h"

VulkanFrame::VulkanFrame(VkImageView color_image_view, const VkRect2D& render_area) :
    m_color_image_view{ color_image_view },
    m_render_area{ render_area }
{
}

void VulkanFrame::setMsaaImageView(VkImageView image_view)
{
    m_msaa_image_view = image_view;
}

void VulkanFrame::setDepthImageView(VkImageView image_view)
{
    m_depth_image_view = image_view;
}

void VulkanFrame::setStencilImageView(VkImageView image_view)
{
    m_stencil_image_view = image_view;
}

void VulkanFrame::setClearColor(float red, float green, float blue, float alpha)
{
    m_clear_color = { red, green, blue, alpha };
}

void VulkanFrame::setClearColor(const float4& clear_color)
{
    m_clear_color = clear_color;
}

void VulkanFrame::setClearDepth(float depth)
{
    m_clear_depth = depth;
}

void VulkanFrame::setClearStencil(uint32_t stencil)
{
    m_clear_stencil = stencil;
}

void VulkanFrame::beginRendering(VkCommandBuffer command_buffer) const
{
    VkRenderingAttachmentInfo color_rendering_attachment_info{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
    VkRenderingAttachmentInfo depth_rendering_attachment_info{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
    VkRenderingAttachmentInfo stencil_rendering_attachment_info{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
    VkRenderingInfo rendering_info{ VK_STRUCTURE_TYPE_RENDERING_INFO };

    color_rendering_attachment_info.imageView = m_color_image_view;
    color_rendering_attachment_info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    color_rendering_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_rendering_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_rendering_attachment_info.clearValue.color = { m_clear_color[0], m_clear_color[1], m_clear_color[2], m_clear_color[3] };

    if (m_msaa_image_view != VK_NULL_HANDLE)
    {
        color_rendering_attachment_info.imageView = m_msaa_image_view;
        color_rendering_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        color_rendering_attachment_info.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
        color_rendering_attachment_info.resolveImageView = m_color_image_view;
        color_rendering_attachment_info.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    if (m_depth_image_view != VK_NULL_HANDLE)
    {
        depth_rendering_attachment_info.imageView = m_depth_image_view;
        depth_rendering_attachment_info.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_rendering_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_rendering_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_rendering_attachment_info.clearValue.depthStencil.depth = m_clear_depth;

        rendering_info.pDepthAttachment = &depth_rendering_attachment_info;
    }

    if (m_stencil_image_view != VK_NULL_HANDLE)
    {
        stencil_rendering_attachment_info.imageView = m_stencil_image_view;
        stencil_rendering_attachment_info.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        stencil_rendering_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        stencil_rendering_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        stencil_rendering_attachment_info.clearValue.depthStencil.stencil = m_clear_stencil;

        rendering_info.pStencilAttachment = &stencil_rendering_attachment_info;
    }

    rendering_info.renderArea = m_render_area;
    rendering_info.layerCount = 1u;
    rendering_info.colorAttachmentCount = 1u;
    rendering_info.pColorAttachments = &color_rendering_attachment_info;

    vkCmdBeginRendering(command_buffer, &rendering_info);
}
