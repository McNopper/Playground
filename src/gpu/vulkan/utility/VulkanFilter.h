#ifndef GPU_VULKAN_UTILITY_VULKANFILTER_H_
#define GPU_VULKAN_UTILITY_VULKANFILTER_H_

#include <cstdint>
#include <initializer_list>
#include <vector>

#include <volk.h>

#include "core/templates/Filter.h"

class PhysicalDeviceTypePriorityFilter : public Filter<VkPhysicalDevice>
{

private:

    std::vector<VkPhysicalDeviceType> m_physical_device_types{};

public:

    PhysicalDeviceTypePriorityFilter(std::initializer_list<VkPhysicalDeviceType> physical_device_types);

    void algorithm() override;

};

//

class PhysicalDeviceTypeFilter : public Filter<VkPhysicalDevice>
{

private:

    VkPhysicalDeviceType m_physical_device_type{ VK_PHYSICAL_DEVICE_TYPE_OTHER };

public:

    explicit PhysicalDeviceTypeFilter(VkPhysicalDeviceType physical_device_type);

    void algorithm() override;

};

//

class PhysicalDeviceExtensionNameFilter : public Filter<VkPhysicalDevice>
{

private:

    std::vector<const char*> m_extension_names{};

public:

    PhysicalDeviceExtensionNameFilter(std::initializer_list<const char*> extension_names);

    void algorithm() override;

};

//

class QueueFamilyIndexFlagsFilter : public Filter<uint32_t>
{

private:

    VkQueueFlags m_queue_flags{ 0u };

    const std::vector<VkQueueFamilyProperties2>& m_queue_family_properties2{};

public:

    QueueFamilyIndexFlagsFilter(VkQueueFlags queue_flags, const std::vector<VkQueueFamilyProperties2>& queue_family_properties2);
    
    void algorithm() override;

};

class QueueFamilyIndexSurfaceFilter : public Filter<uint32_t>
{

private:

    VkPhysicalDevice m_physical_device{ VK_NULL_HANDLE };

    VkSurfaceKHR m_surface{ VK_NULL_HANDLE };

public:

    QueueFamilyIndexSurfaceFilter(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

    void algorithm() override;

};

class MemoryTypeIndexBitsFilter : public Filter<uint32_t>
{

private:

    uint32_t m_memory_type_bits{};

public:

    explicit MemoryTypeIndexBitsFilter(uint32_t memory_type_bits);

    void algorithm() override;

};

class MemoryTypeIndexPropertyFlagsFilter : public Filter<uint32_t>
{

private:

    VkMemoryPropertyFlags m_memory_property_flags{};

    VkPhysicalDeviceMemoryProperties m_physical_device_memory_properties{};

public:

    MemoryTypeIndexPropertyFlagsFilter(VkMemoryPropertyFlags memory_property_flags, VkPhysicalDeviceMemoryProperties physical_device_memory_properties);

    void algorithm() override;

};


class FormatImageFilter : public Filter<VkFormat>
{

private:

    VkPhysicalDevice m_physical_device{ VK_NULL_HANDLE };

    VkImageTiling m_image_tiling{ VK_IMAGE_TILING_OPTIMAL };

    VkFormatFeatureFlags m_format_feature_flags{};

public:

    FormatImageFilter(VkPhysicalDevice physical_device, VkImageTiling image_tiling, VkFormatFeatureFlags format_feature_flags);

    void algorithm() override;

};

//

class SurfaceFormatFilter : public Filter<VkSurfaceFormatKHR>
{

private:

    VkSurfaceFormatKHR m_surface_format{};

public:

    explicit SurfaceFormatFilter(VkSurfaceFormatKHR surface_format);

    void algorithm() override;

};

class PresentModeFilter : public Filter<VkPresentModeKHR>
{

private:

    VkPresentModeKHR m_present_mode{};

public:

    explicit PresentModeFilter(VkPresentModeKHR present_mode);

    void algorithm() override;

};

#endif /* GPU_VULKAN_UTILITY_VULKANFILTER_H_ */