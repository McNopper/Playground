#include "vulkan_query.h"

std::vector<VkLayerProperties> gatherInstanceLayerProperties()
{
    std::vector<VkLayerProperties> layer_properties{};

    uint32_t layer_count{ 0u };
    auto result = vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    if (result != VK_SUCCESS || layer_count == 0)
    {
        return layer_properties;
    }

    layer_properties.resize(layer_count);

    vkEnumerateInstanceLayerProperties(&layer_count, layer_properties.data());

    return layer_properties;
}

std::vector<VkExtensionProperties> gatherInstanceExtensionProperties(const char* layer_name)
{
    std::vector<VkExtensionProperties> extension_properties{};

    uint32_t extension_count{ 0u };
    auto result = vkEnumerateInstanceExtensionProperties(layer_name, &extension_count, nullptr);
    if (result != VK_SUCCESS || extension_count == 0u)
    {
        return extension_properties;
    }

    extension_properties.resize(extension_count);

    vkEnumerateInstanceExtensionProperties(layer_name, &extension_count, extension_properties.data());

    return extension_properties;
}

std::vector<VkPhysicalDevice> gatherPhysicalDevices(VkInstance instance)
{
    std::vector<VkPhysicalDevice> physical_devices{};

    uint32_t physical_device_count{ 0u };
    auto result = vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr);
    if (result != VK_SUCCESS || physical_device_count == 0u)
    {
        return physical_devices;
    }

    physical_devices.resize(physical_device_count);

    vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices.data());

    return physical_devices;
}

std::vector<VkExtensionProperties> gatherPhysicalDeviceExtensionProperties(VkPhysicalDevice physical_device, const char* layer_name)
{
    std::vector<VkExtensionProperties> extension_properties{};

    uint32_t extension_count{ 0u };
    auto result = vkEnumerateDeviceExtensionProperties(physical_device, layer_name, &extension_count, nullptr);
    if (result != VK_SUCCESS || extension_count == 0u)
    {
        return extension_properties;
    }

    extension_properties.resize(extension_count);

    vkEnumerateDeviceExtensionProperties(physical_device, layer_name, &extension_count, extension_properties.data());

    return extension_properties;
}

std::vector<VkQueueFamilyProperties2> gatherPhysicalDeviceQueueFamilyProperties2(VkPhysicalDevice physical_device)
{
    std::vector<VkQueueFamilyProperties2> queue_family_properties2{};

    uint32_t queue_family_property_count{ 0u };
    vkGetPhysicalDeviceQueueFamilyProperties2(physical_device, &queue_family_property_count, nullptr);
    if (queue_family_property_count == 0u)
    {
        return queue_family_properties2;
    }

    queue_family_properties2.resize(queue_family_property_count);
    for (auto& queue_family_property2 : queue_family_properties2)
    {
        queue_family_property2.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
    }

    vkGetPhysicalDeviceQueueFamilyProperties2(physical_device, &queue_family_property_count, queue_family_properties2.data());

    return queue_family_properties2;
}

VkPhysicalDeviceProperties2 gatherPhysicalDeviceProperties2(VkPhysicalDevice physical_device)
{
    VkPhysicalDeviceProperties2 physical_device_properties2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };

    vkGetPhysicalDeviceProperties2(physical_device, &physical_device_properties2);

    return physical_device_properties2;
}

VkPhysicalDeviceFeatures2 gatherPhysicalDeviceFeatures2(VkPhysicalDevice physical_device)
{
    VkPhysicalDeviceFeatures2 physical_device_features2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };

    vkGetPhysicalDeviceFeatures2(physical_device, &physical_device_features2);

    return physical_device_features2;
}

VkPhysicalDeviceVulkan11Features gatherPhysicalDeviceVulkan11Features(VkPhysicalDevice physical_device)
{
    VkPhysicalDeviceVulkan11Features physical_device_vulkan11_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES };

    VkPhysicalDeviceFeatures2 physical_device_features2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
    physical_device_features2.pNext = &physical_device_vulkan11_features;

    vkGetPhysicalDeviceFeatures2(physical_device, &physical_device_features2);

    return physical_device_vulkan11_features;
}

VkPhysicalDeviceVulkan12Features gatherPhysicalDeviceVulkan12Features(VkPhysicalDevice physical_device)
{
    VkPhysicalDeviceVulkan12Features physical_device_vulkan12_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };

    VkPhysicalDeviceFeatures2 physical_device_features2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
    physical_device_features2.pNext = &physical_device_vulkan12_features;

    vkGetPhysicalDeviceFeatures2(physical_device, &physical_device_features2);

    return physical_device_vulkan12_features;
}

VkPhysicalDeviceVulkan13Features gatherPhysicalDeviceVulkan13Features(VkPhysicalDevice physical_device)
{
    VkPhysicalDeviceVulkan13Features physical_device_vulkan13_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };

    VkPhysicalDeviceFeatures2 physical_device_features2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
    physical_device_features2.pNext = &physical_device_vulkan13_features;

    vkGetPhysicalDeviceFeatures2(physical_device, &physical_device_features2);

    return physical_device_vulkan13_features;
}

VkPhysicalDeviceVulkan14Features gatherPhysicalDeviceVulkan14Features(VkPhysicalDevice physical_device)
{
    VkPhysicalDeviceVulkan14Features physical_device_vulkan14_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES };

    VkPhysicalDeviceFeatures2 physical_device_features2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
    physical_device_features2.pNext = &physical_device_vulkan14_features;

    vkGetPhysicalDeviceFeatures2(physical_device, &physical_device_features2);

    return physical_device_vulkan14_features;
}

VkPhysicalDeviceMemoryProperties2 gatherPhysicalDeviceMemoryProperties2(VkPhysicalDevice physical_device)
{
    VkPhysicalDeviceMemoryProperties2 physical_device_memory_properties2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2 };
    vkGetPhysicalDeviceMemoryProperties2(physical_device, &physical_device_memory_properties2);

    return physical_device_memory_properties2;
}

VkFormatProperties2 gatherPhysicalDeviceFormatProperties2(VkPhysicalDevice physical_device, VkFormat format)
{
    VkFormatProperties2 format_properties2{ VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2 };

    vkGetPhysicalDeviceFormatProperties2(physical_device, format, &format_properties2);

    return format_properties2;
}

VkPhysicalDeviceDescriptorBufferPropertiesEXT gatherPhysicalDeviceDescriptorBufferPropertiesEXT(VkPhysicalDevice physical_device)
{
    VkPhysicalDeviceDescriptorBufferPropertiesEXT descriptor_buffer_properties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_PROPERTIES_EXT };

    VkPhysicalDeviceProperties2 physical_device_properties2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
    physical_device_properties2.pNext = &descriptor_buffer_properties;

    vkGetPhysicalDeviceProperties2(physical_device, &physical_device_properties2);

    return descriptor_buffer_properties;
}

VkMemoryRequirements2 gatherDeviceImageMemoryRequirements2(VkDevice device, VkImage image)
{
    VkImageMemoryRequirementsInfo2 image_memory_requirements_info2{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2 };
    image_memory_requirements_info2.image = image;

    VkMemoryRequirements2 memory_requirements2{ VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2 };
    vkGetImageMemoryRequirements2(device, &image_memory_requirements_info2, &memory_requirements2);

    return memory_requirements2;
}

VkMemoryRequirements2 gatherDeviceBufferMemoryRequirements2(VkDevice device, VkBuffer buffer)
{
    VkBufferMemoryRequirementsInfo2 buffer_memory_requirements_info2{ VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2 };
    buffer_memory_requirements_info2.buffer = buffer;

    VkMemoryRequirements2 memory_requirements2{ VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2 };
    vkGetBufferMemoryRequirements2(device, &buffer_memory_requirements_info2, &memory_requirements2);

    return memory_requirements2;
}

//

VkBool32 gatherPhysicalDeviceSurfaceSupport(VkPhysicalDevice physical_device, uint32_t queue_family_index, VkSurfaceKHR surface)
{
    VkBool32 supported{ VK_FALSE };

    VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, queue_family_index, surface, &supported);

    return (result == VK_SUCCESS) && (supported == VK_TRUE);
}

std::optional<VkSurfaceCapabilitiesKHR> gatherPhysicalDeviceSurfaceCapabilities(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
    VkSurfaceCapabilitiesKHR surface_capabilities{};

    VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities);
    if (result == VK_SUCCESS)
    {
        return surface_capabilities;
    }

    return {};
}

std::vector<VkSurfaceFormatKHR> gatherPhysicalDeviceSurfaceFormats(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
    std::vector<VkSurfaceFormatKHR> surface_formats{};

    uint32_t surface_format_count{ 0u };
    auto result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, nullptr);
    if (result != VK_SUCCESS || surface_format_count == 0u)
    {
        return surface_formats;
    }

    surface_formats.resize(surface_format_count);

    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, surface_formats.data());

    return surface_formats;
}

std::vector<VkPresentModeKHR> gatherPhysicalDeviceSurfacePresentModes(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
    std::vector<VkPresentModeKHR> present_modes{};

    uint32_t present_mode_count{ 0u };
    auto result = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr);
    if (result != VK_SUCCESS || present_mode_count == 0u)
    {
        return present_modes;
    }

    present_modes.resize(present_mode_count);

    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, present_modes.data());

    return present_modes;
}

std::vector<VkImage> gatherDeviceSwapchainImages(VkDevice device, VkSwapchainKHR swapchain)
{
    std::vector<VkImage> swapchain_images{};

    uint32_t swapchain_image_count{ 0u };
    auto result = vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, nullptr);
    if (result != VK_SUCCESS || swapchain_image_count == 0u)
    {
        return swapchain_images;
    }

    swapchain_images.resize(swapchain_image_count);

    vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, swapchain_images.data());

    return swapchain_images;
}
