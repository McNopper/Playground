#include "Texture2D.h"

#include "gpu/gpu.h"

Texture2D::Texture2D(VkPhysicalDevice physical_device, VkDevice device) :
    Texture(physical_device, device)
{
    m_image_type = VK_IMAGE_TYPE_2D;
    m_image_view_type = VK_IMAGE_VIEW_TYPE_2D;
    m_extent.depth = 1u;
}

Texture2D::~Texture2D()
{
}

void Texture2D::setWidth(uint32_t width)
{
    m_extent.width = width;
}

void Texture2D::setHeight(uint32_t height)
{
    m_extent.height = height;
}

void Texture2D::setExtent(uint32_t width, uint32_t height)
{
    m_extent.width = width;
    m_extent.height = height;
    m_extent.depth = 1u;
}

bool Texture2D::create()
{
    return createImage();
}

bool Texture2D::upload(const ImageData& image_data, uint32_t mip_level)
{
    if (!isValid())
    {
        return false;
    }

    uint32_t expected_width = std::max(m_extent.width >> mip_level, 1u);
    uint32_t expected_height = std::max(m_extent.height >> mip_level, 1u);
    if (image_data.width != expected_width || image_data.height != expected_height)
    {
        return false;
    }

    VkFormat expected_format = getVulkanFormat(image_data);
    if (expected_format != m_format)
    {
        return false;
    }

    if (mip_level >= m_mip_levels)
    {
        return false;
    }

    VkImageSubresourceLayers subresource_layers{};
    subresource_layers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource_layers.mipLevel = mip_level;
    subresource_layers.baseArrayLayer = 0u;
    subresource_layers.layerCount = 1u;

    hostTransitionImageLayout(m_device, m_image_resource.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, mip_level, 1u);
    copyHostToImage(m_device, image_data.pixels.data(), 0u, 0u, m_image_resource.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, { expected_width, expected_height, 1u }, subresource_layers);
    hostTransitionImageLayout(m_device, m_image_resource.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, mip_level, 1u);

    return true;
}

uint32_t Texture2D::getWidth() const
{
    return m_extent.width;
}

uint32_t Texture2D::getHeight() const
{
    return m_extent.height;
}

bool Texture2D::uploadMipMaps(const std::vector<ImageData>& mip_levels)
{
    if (static_cast<uint32_t>(mip_levels.size()) != m_mip_levels)
    {
        return false;
    }

    for (uint32_t i{ 0u }; i < static_cast<uint32_t>(mip_levels.size()); ++i)
    {
        if (!upload(mip_levels[i], i))
        {
            return false;
        }
    }

    return true;
}
