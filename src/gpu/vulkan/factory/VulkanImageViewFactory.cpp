#include "VulkanImageViewFactory.h"

VulkanImageViewFactory::VulkanImageViewFactory(VkDevice device, VkImage image, VkImageViewType image_view_type, VkFormat format, const VkImageSubresourceRange& image_subresource_range) :
    m_device{ device },
    m_image{ image },
    m_image_view_type{ image_view_type },
    m_format{ format },
    m_image_subresource_range{ image_subresource_range }
{
}

VkImageView VulkanImageViewFactory::create() const
{
    VkImageViewCreateInfo image_view_create_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    image_view_create_info.flags = m_image_view_create_flags;
    image_view_create_info.image = m_image;
    image_view_create_info.viewType = m_image_view_type;
    image_view_create_info.format = m_format;
    image_view_create_info.components = m_component_mapping;
    image_view_create_info.subresourceRange = m_image_subresource_range;

    VkImageView image_view{ VK_NULL_HANDLE };
    vkCreateImageView(m_device, &image_view_create_info, nullptr, &image_view);

    return image_view;
}
