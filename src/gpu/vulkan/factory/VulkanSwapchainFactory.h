#ifndef GPU_VULKAN_FACTORY_VULKANSWAPCHAINFACTORY_H_
#define GPU_VULKAN_FACTORY_VULKANSWAPCHAINFACTORY_H_

#include <cstdint>
#include <vector>

#include <volk.h>

class VulkanSwapchainFactory
{

private:

    VkPhysicalDevice m_physical_device{ VK_NULL_HANDLE };
    VkDevice m_device{ VK_NULL_HANDLE };
    uint32_t m_target_min_image_count{ 3u };

    VkSwapchainCreateFlagsKHR m_swapchain_create_flags{ 0u };
    VkSurfaceKHR m_surface{ VK_NULL_HANDLE };
    VkSurfaceFormatKHR m_surface_format{};
    uint32_t m_image_array_layers{ 1u };
    VkImageUsageFlags m_image_usage_flags{ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
    VkSharingMode m_image_sharing_mode{ VK_SHARING_MODE_EXCLUSIVE };
    std::vector<uint32_t> m_queue_family_indices{};
    VkSurfaceTransformFlagBitsKHR m_pre_transform{ VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR };
    VkCompositeAlphaFlagBitsKHR m_composite_alpha{ VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR };
    VkPresentModeKHR m_present_mode{};
    VkBool32 m_clipped{ VK_TRUE };
    VkSwapchainKHR m_old_swapchain{ nullptr };

public:

    VulkanSwapchainFactory() = delete;

    VulkanSwapchainFactory(VkPhysicalDevice physical_device, VkDevice device, VkSurfaceKHR surface, const VkSurfaceFormatKHR& surface_format, VkPresentModeKHR present_mode);

    void setTargetMinImageCount(uint32_t target_min_image_count);

    void setOldSwapchain(VkSwapchainKHR old_swapchain);

    VkSwapchainKHR create() const;

};

#endif /* GPU_VULKAN_FACTORY_VULKANSWAPCHAINFACTORY_H_ */
