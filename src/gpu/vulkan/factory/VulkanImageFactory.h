#ifndef GPU_VULKAN_FACTORY_VULKANIMAGEFACTORY_H_
#define GPU_VULKAN_FACTORY_VULKANIMAGEFACTORY_H_

#include <cstdint>
#include <vector>

#include <volk.h>

class VulkanImageFactory
{

private:

    VkDevice m_device{ VK_NULL_HANDLE };

    VkImageCreateFlags m_flags{};
    VkImageType m_image_type{ VK_IMAGE_TYPE_2D };
    VkFormat m_format{ VK_FORMAT_UNDEFINED };
    VkExtent3D m_extent{ 0u, 0u, 0u };
    uint32_t m_mip_levels{ 1u };
    uint32_t m_array_layers{ 1u };
    VkSampleCountFlagBits m_samples{ VK_SAMPLE_COUNT_1_BIT };
    VkImageTiling m_tiling{ VK_IMAGE_TILING_OPTIMAL };
    VkImageUsageFlags m_usage{};
    VkSharingMode m_sharing_mode{};
    std::vector<uint32_t> m_queue_family_indices{};
    VkImageLayout m_initial_layout{ VK_IMAGE_LAYOUT_UNDEFINED };

public:

    VulkanImageFactory() = delete;

    VulkanImageFactory(VkDevice device, VkFormat format, const VkExtent3D& extent, VkSampleCountFlagBits samples, VkImageUsageFlags usage);

    void setMipLevels(uint32_t mip_levels);
    void setArrayLayers(uint32_t array_layers);
    void setFlags(VkImageCreateFlags flags);

    VkImage create() const;
};

#endif /* GPU_VULKAN_FACTORY_VULKANIMAGEFACTORY_H_ */
