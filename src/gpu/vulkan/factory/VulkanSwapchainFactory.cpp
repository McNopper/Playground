#include "VulkanSwapchainFactory.h"

#include "gpu/vulkan/utility/vulkan_query.h"

VulkanSwapchainFactory::VulkanSwapchainFactory(VkPhysicalDevice physical_device, VkDevice device, VkSurfaceKHR surface, const VkSurfaceFormatKHR& surface_format, VkPresentModeKHR present_mode) :
    m_physical_device{ physical_device },
    m_device{ device },
    m_surface{ surface },
    m_surface_format{ surface_format },
    m_present_mode{ present_mode }
{
}

void VulkanSwapchainFactory::setTargetMinImageCount(uint32_t target_min_image_count)
{
    m_target_min_image_count = target_min_image_count;
}

void VulkanSwapchainFactory::setSwapchainCreateFlags(VkSwapchainCreateFlagsKHR flags)
{
    m_swapchain_create_flags = flags;
}

void VulkanSwapchainFactory::setOldSwapchain(VkSwapchainKHR old_swapchain)
{
    m_old_swapchain = old_swapchain;
}

VkSwapchainKHR VulkanSwapchainFactory::create() const
{
    VkSwapchainKHR swapchain{ VK_NULL_HANDLE };

    auto surface_capabilities = gatherPhysicalDeviceSurfaceCapabilities(m_physical_device, m_surface);
    if (!surface_capabilities.has_value())
    {
        return swapchain;
    }

    uint32_t min_image_count{ m_target_min_image_count };
    if (surface_capabilities->minImageCount > min_image_count)
    {
        min_image_count = surface_capabilities->minImageCount;
    }
    if (surface_capabilities->maxImageCount != 0u && surface_capabilities->maxImageCount < min_image_count)
    {
        min_image_count = surface_capabilities->maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchain_create_info{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    swapchain_create_info.flags = m_swapchain_create_flags;
    swapchain_create_info.surface = m_surface;
    swapchain_create_info.minImageCount = min_image_count;
    swapchain_create_info.imageFormat = m_surface_format.format;
    swapchain_create_info.imageColorSpace = m_surface_format.colorSpace;
    swapchain_create_info.imageExtent = surface_capabilities->currentExtent;
    swapchain_create_info.imageArrayLayers = m_image_array_layers;
    swapchain_create_info.imageUsage = m_image_usage_flags;
    swapchain_create_info.imageSharingMode = m_image_sharing_mode;
    swapchain_create_info.queueFamilyIndexCount = (uint32_t)m_queue_family_indices.size();
    if (!m_queue_family_indices.empty())
    {
        swapchain_create_info.pQueueFamilyIndices = m_queue_family_indices.data();
    }
    swapchain_create_info.preTransform = m_pre_transform;
    swapchain_create_info.compositeAlpha = m_composite_alpha;
    swapchain_create_info.presentMode = m_present_mode;
    swapchain_create_info.clipped = m_clipped;
    swapchain_create_info.oldSwapchain = m_old_swapchain;

    vkCreateSwapchainKHR(m_device, &swapchain_create_info, nullptr, &swapchain);

    return swapchain;
}
