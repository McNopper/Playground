#include "Texture.h"

#include "gpu/gpu.h"

Texture::Texture(VkPhysicalDevice physical_device, VkDevice device) :
	m_physical_device{ physical_device }, m_device{ device }
{
}

Texture::~Texture()
{
	Texture::destroy();
}

void Texture::setFormat(VkFormat format)
{
	m_format = format;
}

void Texture::setUsage(VkImageUsageFlags usage)
{
	m_usage = usage;
}

void Texture::setMipLevels(uint32_t mip_levels)
{
	m_mip_levels = mip_levels;
}

void Texture::setImageCreateFlags(VkImageCreateFlags flags)
{
	m_image_create_flags = flags;
}

bool Texture::createImage()
{
	destroy();

	if (m_extent.width == 0u || m_extent.height == 0u || m_extent.depth == 0u)
	{
		return false;
	}

	if (m_format == VK_FORMAT_UNDEFINED)
	{
		return false;
	}

	if (m_usage == 0u)
	{
		return false;
	}

	VkImageUsageFlags usage = m_usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	VulkanImageFactory image_factory{ m_device, m_format, m_extent, VK_SAMPLE_COUNT_1_BIT, usage };
	image_factory.setMipLevels(m_mip_levels);
	image_factory.setArrayLayers(m_array_layers);
	image_factory.setFlags(m_image_create_flags);
	m_image_resource.image = image_factory.create();
	if (m_image_resource.image == VK_NULL_HANDLE)
	{
		return false;
	}

	m_image_resource.device_memory = buildImageDeviceMemory(m_physical_device, m_device, m_image_resource.image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	if (m_image_resource.device_memory == VK_NULL_HANDLE)
	{
		return false;
	}

	VkImageSubresourceRange subresource_range{};
	subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresource_range.baseMipLevel = 0u;
	subresource_range.levelCount = m_mip_levels;
	subresource_range.baseArrayLayer = 0u;
	subresource_range.layerCount = m_array_layers;

	VulkanImageViewFactory image_view_factory{ m_device, m_image_resource.image, m_image_view_type, m_format, subresource_range };
	m_image_resource.image_view = image_view_factory.create();
	if (m_image_resource.image_view == VK_NULL_HANDLE)
	{
		return false;
	}

	return true;
}

VkImage Texture::getImage() const
{
	return m_image_resource.image;
}

VkImageView Texture::getImageView() const
{
	return m_image_resource.image_view;
}

VkImageType Texture::getImageType() const
{
	return m_image_type;
}

VkImageViewType Texture::getImageViewType() const
{
	return m_image_view_type;
}

VkExtent3D Texture::getExtent() const
{
	return m_extent;
}

VkFormat Texture::getFormat() const
{
	return m_format;
}

VkImageUsageFlags Texture::getUsage() const
{
	return m_usage;
}

uint32_t Texture::getMipLevels() const
{
	return m_mip_levels;
}

uint32_t Texture::getArrayLayers() const
{
	return m_array_layers;
}

bool Texture::isValid() const
{
	return m_image_resource.image != VK_NULL_HANDLE && 
	       m_image_resource.device_memory != VK_NULL_HANDLE && 
	       m_image_resource.image_view != VK_NULL_HANDLE;
}

void Texture::destroy()
{
	destroyResource(m_device, m_image_resource);
}
