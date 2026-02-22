#ifndef GPU_VULKAN_FACTORY_VULKANIMAGEVIEWFACTORY_H_
#define GPU_VULKAN_FACTORY_VULKANIMAGEVIEWFACTORY_H_

#include <volk.h>

class VulkanImageViewFactory
{

private:

    VkDevice m_device{ VK_NULL_HANDLE };

    VkImageViewCreateFlags m_image_view_create_flags{ 0u };
    VkImage m_image{ VK_NULL_HANDLE };
    VkImageViewType m_image_view_type{ VK_IMAGE_VIEW_TYPE_1D };
    VkFormat m_format{ VK_FORMAT_UNDEFINED };
    VkComponentMapping m_component_mapping{};
    VkImageSubresourceRange m_image_subresource_range{};

public:

    VulkanImageViewFactory() = delete;

    VulkanImageViewFactory(VkDevice device, VkImage image, VkImageViewType image_view_type, VkFormat format, const VkImageSubresourceRange& image_subresource_range);

    VkImageView create() const;
};

#endif /* GPU_VULKAN_FACTORY_VULKANIMAGEVIEWFACTORY_H_ */
