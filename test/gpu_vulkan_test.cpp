#include <gtest/gtest.h>

#include <algorithm>

#include "gpu/gpu.h"

namespace {

struct VulkanHandles {
    VkInstance instance{ VK_NULL_HANDLE };
    VkPhysicalDevice physical_device{ VK_NULL_HANDLE };
    std::uint32_t queue_family_index{ 0u };
    VkDevice device{ VK_NULL_HANDLE };
    VkQueue queue{VK_NULL_HANDLE};
};

bool initVulkan(VulkanHandles& handles)
{
    VulkanInstanceFactory vulkan_instance_factory{};
    vulkan_instance_factory.addEnabledLayerName("VK_LAYER_KHRONOS_validation");
    handles.instance = vulkan_instance_factory.create();
    if (handles.instance == VK_NULL_HANDLE)
    {
        return false;
    }

    auto physical_devices = gatherPhysicalDevices(handles.instance);
    physical_devices = PhysicalDeviceTypePriorityFilter{ VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU, VK_PHYSICAL_DEVICE_TYPE_CPU } << physical_devices;
    if (physical_devices.empty())
    {
        return false;
    }
    handles.physical_device = physical_devices[0u];

    auto queue_family_properties = gatherPhysicalDeviceQueueFamilyProperties2(handles.physical_device);
    std::vector<std::uint32_t> queue_family_indices(queue_family_properties.size());
    std::generate(queue_family_indices.begin(), queue_family_indices.end(), [index = 0u]() mutable { return index++; });

    queue_family_indices = QueueFamilyIndexFlagsFilter{VK_QUEUE_GRAPHICS_BIT, queue_family_properties} << queue_family_indices;
    if (queue_family_indices.empty())
    {
        return false;
    }
    handles.queue_family_index = queue_family_indices[0u];

    VulkanDeviceFactory vulkan_device_factory{ handles.physical_device, handles.queue_family_index };
    handles.device = vulkan_device_factory.create();
    if (handles.device == VK_NULL_HANDLE)
    {
        return false;
    }

    vkGetDeviceQueue(handles.device, handles.queue_family_index, 0, &handles.queue);

    return true;
}

void terminateVulkan(VulkanHandles& handles)
{
    handles.queue = VK_NULL_HANDLE;

    if (handles.device != VK_NULL_HANDLE)
    {
        vkDestroyDevice(handles.device, nullptr);

        handles.device = VK_NULL_HANDLE;
    }

    handles.queue_family_index = 0u;

    handles.physical_device = VK_NULL_HANDLE;

    if (handles.instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(handles.instance, nullptr);

        handles.instance = VK_NULL_HANDLE;
    }
}

}

TEST(TestVulkan, InitTerminate)
{
    VulkanSetup vulkan_setup{};

    auto result = vulkan_setup.init();
    EXPECT_TRUE(result);
    if (!result)
    {
        return;
    }
        
    VulkanHandles handles{};

    result = initVulkan(handles);
    EXPECT_TRUE(result);
    if (result)
    {
        EXPECT_NE(handles.instance, VK_NULL_HANDLE);
        EXPECT_NE(handles.physical_device, VK_NULL_HANDLE);
        // Not checking queue family index
        EXPECT_NE(handles.device, VK_NULL_HANDLE);
        EXPECT_NE(handles.queue, VK_NULL_HANDLE);
    }

    terminateVulkan(handles);
    EXPECT_EQ(handles.instance, VK_NULL_HANDLE);
    EXPECT_EQ(handles.physical_device, VK_NULL_HANDLE);
    EXPECT_EQ(handles.queue_family_index, 0u);
    EXPECT_EQ(handles.device, VK_NULL_HANDLE);
    EXPECT_EQ(handles.queue, VK_NULL_HANDLE);

    vulkan_setup.terminate();    
}
