#include "VulkanImageFactory.h"

VulkanImageFactory::VulkanImageFactory(VkDevice device, VkFormat format, const VkExtent3D& extent, VkSampleCountFlagBits samples, VkImageUsageFlags usage) :
    m_device{ device }, m_format{ format }, m_extent{ extent }, m_samples{ samples }, m_usage{ usage }
{
}

void VulkanImageFactory::setMipLevels(uint32_t mip_levels)
{
    m_mip_levels = mip_levels;
}

void VulkanImageFactory::setArrayLayers(uint32_t array_layers)
{
    m_array_layers = array_layers;
}

void VulkanImageFactory::setFlags(VkImageCreateFlags flags)
{
    m_flags = flags;
}

VkImage VulkanImageFactory::create() const
{
    VkImageCreateInfo image_create_info{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    image_create_info.flags = m_flags;
    image_create_info.imageType = m_image_type;
    image_create_info.format = m_format;
    image_create_info.extent = m_extent;
    image_create_info.mipLevels = m_mip_levels;
    image_create_info.arrayLayers = m_array_layers;
    image_create_info.samples = m_samples;
    image_create_info.tiling = m_tiling;
    image_create_info.usage = m_usage;
    image_create_info.sharingMode = m_sharing_mode;
    if (!m_queue_family_indices.empty())
    {
        image_create_info.queueFamilyIndexCount = (uint32_t)m_queue_family_indices.size();
        image_create_info.pQueueFamilyIndices = m_queue_family_indices.data();
    }
    image_create_info.initialLayout = m_initial_layout;

    VkImage image{ VK_NULL_HANDLE };
    vkCreateImage(m_device, &image_create_info, nullptr, &image);

    return image;
}
