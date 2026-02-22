#include "TextureCube.h"

#include "gpu/gpu.h"

TextureCube::TextureCube(VkPhysicalDevice physical_device, VkDevice device) :
	Texture(physical_device, device)
{
	m_image_type         = VK_IMAGE_TYPE_2D;
	m_image_view_type    = VK_IMAGE_VIEW_TYPE_CUBE;
	m_array_layers       = 6u;
	m_image_create_flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
	m_extent.depth       = 1u;
}

TextureCube::~TextureCube()
{
	if (m_storage_image_view != VK_NULL_HANDLE)
	{
		vkDestroyImageView(m_device, m_storage_image_view, nullptr);
		m_storage_image_view = VK_NULL_HANDLE;
	}
}

void TextureCube::setSize(uint32_t size)
{
	m_extent.width  = size;
	m_extent.height = size;
	m_extent.depth  = 1u;
}

bool TextureCube::create()
{
	if (!createImage())
	{
		return false;
	}

	VkImageSubresourceRange subresource_range{};
	subresource_range.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	subresource_range.baseMipLevel   = 0u;
	subresource_range.levelCount     = m_mip_levels;
	subresource_range.baseArrayLayer = 0u;
	subresource_range.layerCount     = m_array_layers;

	VulkanImageViewFactory view_factory{ m_device, m_image_resource.image, VK_IMAGE_VIEW_TYPE_2D_ARRAY, m_format, subresource_range };
	m_storage_image_view = view_factory.create();

	if (m_storage_image_view == VK_NULL_HANDLE)
	{
		return false;
	}

	return true;
}

VkImageView TextureCube::getStorageImageView() const
{
	return m_storage_image_view;
}

void TextureCube::destroy()
{
	if (m_storage_image_view != VK_NULL_HANDLE)
	{
		vkDestroyImageView(m_device, m_storage_image_view, nullptr);
		m_storage_image_view = VK_NULL_HANDLE;
	}

	Texture::destroy();
}

uint32_t TextureCube::getSize() const
{
	return m_extent.width;
}
