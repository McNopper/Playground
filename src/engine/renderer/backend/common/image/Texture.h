#ifndef ENGINE_RENDERER_BACKEND_COMMON_IMAGE_TEXTURE_H_
#define ENGINE_RENDERER_BACKEND_COMMON_IMAGE_TEXTURE_H_

#include <volk.h>

#include "core/image/image_data.h"
#include "gpu/vulkan/builder/vulkan_resource.h"

/**
 * Texture - Abstract base class for Vulkan image resources
 * 
 * Base class for all texture types (1D, 2D, 3D, Cube).
 * Handles creation of VkImage, VkDeviceMemory, and VkImageView.
 * 
 * Architecture:
 *   Texture (base)
 *   ├── Texture2D
 *   └── TextureCube
 */

class Texture
{

protected:

    VkPhysicalDevice m_physical_device{ VK_NULL_HANDLE };
    VkDevice m_device{ VK_NULL_HANDLE };

    VulkanImageResource m_image_resource{};
    VkImageType m_image_type{ VK_IMAGE_TYPE_2D };
    VkImageViewType m_image_view_type{ VK_IMAGE_VIEW_TYPE_2D };
    VkExtent3D m_extent{ 0u, 0u, 1u };
    VkFormat m_format{ VK_FORMAT_UNDEFINED };
    VkImageUsageFlags m_usage{ 0u };
    uint32_t m_mip_levels{ 1u };
    uint32_t m_array_layers{ 1u };
    VkImageCreateFlags m_image_create_flags{};

    bool createImage();

public:

    Texture() = delete;
    Texture(const Texture& other) = delete;

    Texture(VkPhysicalDevice physical_device, VkDevice device);

    virtual ~Texture();

    Texture& operator=(const Texture& other) = delete;

    void setFormat(VkFormat format);
    void setUsage(VkImageUsageFlags usage);
    void setMipLevels(uint32_t mip_levels);
    void setImageCreateFlags(VkImageCreateFlags flags);

    virtual bool create() = 0;

    VkImage getImage() const;
    VkImageView getImageView() const;
    VkImageType getImageType() const;
    VkImageViewType getImageViewType() const;
    VkExtent3D getExtent() const;
    VkFormat getFormat() const;
    VkImageUsageFlags getUsage() const;
    uint32_t getMipLevels() const;
    uint32_t getArrayLayers() const;

    bool isValid() const;

    virtual void destroy();
};

#endif /* ENGINE_RENDERER_BACKEND_COMMON_IMAGE_TEXTURE_H_ */
