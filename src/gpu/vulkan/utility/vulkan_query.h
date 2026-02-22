#ifndef GPU_VULKAN_UTILITY_VULKANQUERY_H_
#define GPU_VULKAN_UTILITY_VULKANQUERY_H_

#include <cstdint>
#include <optional>
#include <vector>

#include <volk.h>

std::vector<VkLayerProperties> gatherInstanceLayerProperties();

std::vector<VkExtensionProperties> gatherInstanceExtensionProperties(const char* layer_name = nullptr);

std::vector<VkPhysicalDevice> gatherPhysicalDevices(VkInstance instance);

std::vector<VkExtensionProperties> gatherPhysicalDeviceExtensionProperties(VkPhysicalDevice physical_device, const char* layer_name = nullptr);

std::vector<VkQueueFamilyProperties2> gatherPhysicalDeviceQueueFamilyProperties2(VkPhysicalDevice physical_device);

VkPhysicalDeviceProperties2 gatherPhysicalDeviceProperties2(VkPhysicalDevice physical_device);

VkPhysicalDeviceFeatures2 gatherPhysicalDeviceFeatures2(VkPhysicalDevice physical_device);

VkPhysicalDeviceVulkan11Features gatherPhysicalDeviceVulkan11Features(VkPhysicalDevice physical_device);

VkPhysicalDeviceVulkan12Features gatherPhysicalDeviceVulkan12Features(VkPhysicalDevice physical_device);

VkPhysicalDeviceVulkan13Features gatherPhysicalDeviceVulkan13Features(VkPhysicalDevice physical_device);

VkPhysicalDeviceVulkan14Features gatherPhysicalDeviceVulkan14Features(VkPhysicalDevice physical_device);

VkPhysicalDeviceMemoryProperties2 gatherPhysicalDeviceMemoryProperties2(VkPhysicalDevice physical_device);

VkFormatProperties2 gatherPhysicalDeviceFormatProperties2(VkPhysicalDevice physical_device, VkFormat format);

VkPhysicalDeviceDescriptorBufferPropertiesEXT gatherPhysicalDeviceDescriptorBufferPropertiesEXT(VkPhysicalDevice physical_device);

VkMemoryRequirements2 gatherDeviceImageMemoryRequirements2(VkDevice device, VkImage image);

VkMemoryRequirements2 gatherDeviceBufferMemoryRequirements2(VkDevice device, VkBuffer buffer);

// Extensions

VkBool32 gatherPhysicalDeviceSurfaceSupport(VkPhysicalDevice physical_device, uint32_t queue_family_index, VkSurfaceKHR surface);

std::optional<VkSurfaceCapabilitiesKHR> gatherPhysicalDeviceSurfaceCapabilities(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

std::vector<VkSurfaceFormatKHR> gatherPhysicalDeviceSurfaceFormats(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

std::vector<VkPresentModeKHR> gatherPhysicalDeviceSurfacePresentModes(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

std::vector<VkImage> gatherDeviceSwapchainImages(VkDevice device, VkSwapchainKHR swapchain);

#endif /* GPU_VULKAN_UTILITY_VULKANENQUERY_H_ */
