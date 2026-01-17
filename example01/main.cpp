#include <algorithm>
#include <cstdint>
#include <cstdio>

#include <volk.h>

#include "gpu/gpu.h"

int main()
{
    // Setup Vulkan and in this case using Volk.

    printf("Vulkan setup.\n");

    VulkanSetup vulkan_setup{};
    auto result = vulkan_setup.init();
    if (!result)
    {
        printf("Vulkan setup failed\n");

        vulkan_setup.terminate();

        return -1;
    }
    printf("\n");

    // Query and display instance layer and extension properties.

    printf("Instance layer properties:\n");
    auto layer_properties = gatherInstanceLayerProperties();
    for (const auto& layer_property : layer_properties)
    {
        printf("%s %u %u %s\n", layer_property.layerName, layer_property.specVersion, layer_property.implementationVersion, layer_property.description);
    }
    printf("\n");

    printf("Instance extension properties:\n");
    auto extension_properties = gatherInstanceExtensionProperties();
    for (const auto& extension_property : extension_properties)
    {
        printf("%s %u\n", extension_property.extensionName, extension_property.specVersion);
    }
    printf("\n");

    // Create Vulkan instance.

    VulkanInstanceFactory vulkan_instance_factory{};
    vulkan_instance_factory.addEnabledLayerName("VK_LAYER_KHRONOS_validation");
    VkInstance instance = vulkan_instance_factory.create();
    if (instance == VK_NULL_HANDLE)
    {
        printf("Vulkan instance creation failed\n");

        vulkan_setup.terminate();

        return -1;
    }

    //  Query and display physical device properties.

    auto physical_devices = gatherPhysicalDevices(instance);
    printf("Number physical devices: %u\n", (uint32_t)physical_devices.size());

    // Filtering to find a suitable physical device.

    physical_devices = PhysicalDeviceTypePriorityFilter{ VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU, VK_PHYSICAL_DEVICE_TYPE_CPU } << physical_devices;
    if (physical_devices.empty())
    {
        printf("No suitable physical device found.\n");

        // Destroy
        vkDestroyInstance(instance, nullptr);

        vulkan_setup.terminate();

        return -1;
    }
    VkPhysicalDevice physical_device{physical_devices[0u]};
    printf("Found suitable physical device with given filter.\n");
    printf("\n");

    printf("Physical device extension properties:\n");
    extension_properties = gatherPhysicalDeviceExtensionProperties(physical_device);
    for (const auto& extension_property : extension_properties)
    {
        printf("%s %u\n", extension_property.extensionName, extension_property.specVersion);
    }
    printf("\n");

    // Query and display physical device queue family properties.

    auto queue_family_properties = gatherPhysicalDeviceQueueFamilyProperties2(physical_device);
    printf("Number queue family properties: %u\n", (uint32_t)queue_family_properties.size());

    std::vector<uint32_t> queue_family_indices(queue_family_properties.size());
    std::generate(queue_family_indices.begin(), queue_family_indices.end(), [index = 0u]() mutable { return index++; });

    // Filtering to find a suitable queue family index.
    queue_family_indices = QueueFamilyIndexFlagsFilter{VK_QUEUE_GRAPHICS_BIT, queue_family_properties} << queue_family_indices;
    if (queue_family_indices.empty())
    {
        printf("No suitable queue family found.\n");

        // Destroy
        vkDestroyInstance(instance, nullptr);

        vulkan_setup.terminate();

        return -1;
    }
    uint32_t queue_family_index{ queue_family_indices[0u] };
    printf("Found suitable queue family index with given filter.\n");
    printf("\n");

    // Create Vulkan device.

    VulkanDeviceFactory vulkan_device_factory{ physical_device, queue_family_index };
    VkDevice device = vulkan_device_factory.create();
    if (device == VK_NULL_HANDLE)
    {
        printf("Vulkan device creation failed\n");

        // Destroy
        vkDestroyInstance(instance, nullptr);

        vulkan_setup.terminate();

        return -1;
    }
    printf("Vulkan device created.\n");

    VkQueue queue{VK_NULL_HANDLE};
    vkGetDeviceQueue(device, queue_family_index, 0, &queue);
    printf("Vulkan queue retrieved.\n");
    printf("\n");

    // Application logic would go here...

    printf("Application would be executed here.\n");
    printf("\n");

    // Destroy
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);

    // Terminate Vulkan setup.
    vulkan_setup.terminate();

    printf("Vulkan terminated.\n");

	return 0;
}
