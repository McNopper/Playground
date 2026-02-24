#include "TextureCube.h"

#include "gpu/gpu.h"

TextureCube::TextureCube(VkPhysicalDevice physical_device, VkDevice device) :
    Texture(physical_device, device)
{
    m_image_type = VK_IMAGE_TYPE_2D;
    m_image_view_type = VK_IMAGE_VIEW_TYPE_CUBE;
    m_array_layers = 6u;
    m_image_create_flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    m_extent.depth = 1u;
}

TextureCube::~TextureCube()
{
    TextureCube::destroy();
}

void TextureCube::setSize(uint32_t size)
{
    m_extent.width = size;
    m_extent.height = size;
    m_extent.depth = 1u;
}

bool TextureCube::create()
{
    if (!createImage())
    {
        return false;
    }

    // Create one storage image view per mip level.
    // Vulkan requires storage image views to have levelCount = 1.
    m_mip_storage_image_views.resize(m_mip_levels, VK_NULL_HANDLE);
    for (uint32_t mip = 0u; mip < m_mip_levels; ++mip)
    {
        VkImageSubresourceRange subresource_range{};
        subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresource_range.baseMipLevel = mip;
        subresource_range.levelCount = 1u;
        subresource_range.baseArrayLayer = 0u;
        subresource_range.layerCount = m_array_layers;

        VulkanImageViewFactory view_factory{ m_device, m_image_resource.image, VK_IMAGE_VIEW_TYPE_2D_ARRAY, m_format, subresource_range };
        m_mip_storage_image_views[mip] = view_factory.create();

        if (m_mip_storage_image_views[mip] == VK_NULL_HANDLE)
        {
            return false;
        }
    }

    return true;
}

VkImageView TextureCube::getStorageImageView() const
{
    return getStorageImageViewForMip(0u);
}

VkImageView TextureCube::getStorageImageViewForMip(uint32_t mip) const
{
    if (mip >= m_mip_storage_image_views.size())
    {
        return VK_NULL_HANDLE;
    }
    return m_mip_storage_image_views[mip];
}

void TextureCube::destroy()
{
    for (auto& view : m_mip_storage_image_views)
    {
        if (view != VK_NULL_HANDLE)
        {
            vkDestroyImageView(m_device, view, nullptr);
            view = VK_NULL_HANDLE;
        }
    }
    m_mip_storage_image_views.clear();

    Texture::destroy();
}

uint32_t TextureCube::getSize() const
{
    return m_extent.width;
}
