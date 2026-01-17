#include <gtest/gtest.h>

#include <algorithm>
#include <vector>

#include "gpu/gpu.h"

namespace {

struct VulkanHandles {
    VkInstance instance{ VK_NULL_HANDLE };
    VkPhysicalDevice physical_device{ VK_NULL_HANDLE };
    std::uint32_t queue_family_index{ 0u };
    VkDevice device{ VK_NULL_HANDLE };
    VkQueue queue{VK_NULL_HANDLE};
    VkCommandPool command_pool{ VK_NULL_HANDLE };
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

    VulkanCommandPoolFactory command_pool_factory{ handles.device, 0u, handles.queue_family_index };
    handles.command_pool = command_pool_factory.create();
    if (handles.command_pool == VK_NULL_HANDLE)
    {
        return false;
    }

    return true;
}

void terminateVulkan(VulkanHandles& handles)
{
    if (handles.command_pool != VK_NULL_HANDLE)
    {
        vkDestroyCommandPool(handles.device, handles.command_pool, nullptr);
        handles.command_pool = VK_NULL_HANDLE;
    }

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

TEST(TestTransfer, BufferToImageToBufferRoundTrip)
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
    if (!result)
    {
        vulkan_setup.terminate();
        return;
    }

    const std::uint32_t width = 256u;
    const std::uint32_t height = 256u;
    const VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
    const VkDeviceSize buffer_size = width * height * 4u;

    std::vector<std::uint8_t> original_data(buffer_size);
    for (std::size_t i = 0u; i < original_data.size(); i++)
    {
        original_data[i] = static_cast<std::uint8_t>(i % 256);
    }

    auto staging_buffer_upload = createStagingBuffer(handles.physical_device, handles.device, buffer_size);
    EXPECT_TRUE(staging_buffer_upload.has_value());
    if (!staging_buffer_upload.has_value())
    {
        terminateVulkan(handles);
        vulkan_setup.terminate();
        return;
    }

    result = hostToDevice(handles.device, staging_buffer_upload->device_memory, 0u, buffer_size, original_data);
    EXPECT_TRUE(result);

    VulkanImageFactory image_factory{ handles.device, format, {width, height, 1u}, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT };
    VkImage image = image_factory.create();
    EXPECT_NE(image, VK_NULL_HANDLE);
    if (image == VK_NULL_HANDLE)
    {
        destroyResource(handles.device, *staging_buffer_upload);
        terminateVulkan(handles);
        vulkan_setup.terminate();
        return;
    }

    VkDeviceMemory image_memory = buildImageDeviceMemory(handles.physical_device, handles.device, image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    EXPECT_NE(image_memory, VK_NULL_HANDLE);
    if (image_memory == VK_NULL_HANDLE)
    {
        vkDestroyImage(handles.device, image, nullptr);
        destroyResource(handles.device, *staging_buffer_upload);
        terminateVulkan(handles);
        vulkan_setup.terminate();
        return;
    }

    auto staging_buffer_download = createStagingBuffer(handles.physical_device, handles.device, buffer_size);
    EXPECT_TRUE(staging_buffer_download.has_value());
    if (!staging_buffer_download.has_value())
    {
        vkFreeMemory(handles.device, image_memory, nullptr);
        vkDestroyImage(handles.device, image, nullptr);
        destroyResource(handles.device, *staging_buffer_upload);
        terminateVulkan(handles);
        vulkan_setup.terminate();
        return;
    }

    VulkanCommandBufferFactory command_buffer_factory{ handles.device, handles.command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1u };
    auto command_buffers = command_buffer_factory.create();
    EXPECT_FALSE(command_buffers.empty());
    VkCommandBuffer command_buffer = command_buffers[0];

    VkCommandBufferBeginInfo begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    result = vkBeginCommandBuffer(command_buffer, &begin_info);
    EXPECT_EQ(result, VK_SUCCESS);

    transitionImageLayout(command_buffer, image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(command_buffer, staging_buffer_upload->buffer, image, {width, height, 1u});
    transitionImageLayout(command_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    copyImageToBuffer(command_buffer, image, staging_buffer_download->buffer, {width, height, 1u});

    result = vkEndCommandBuffer(command_buffer);
    EXPECT_EQ(result, VK_SUCCESS);

    VkCommandBufferSubmitInfo cmd_buffer_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
    cmd_buffer_info.commandBuffer = command_buffer;

    VkSubmitInfo2 submit_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
    submit_info.commandBufferInfoCount = 1u;
    submit_info.pCommandBufferInfos = &cmd_buffer_info;

    result = vkQueueSubmit2(handles.queue, 1u, &submit_info, VK_NULL_HANDLE);
    EXPECT_EQ(result, VK_SUCCESS);

    result = vkQueueWaitIdle(handles.queue);
    EXPECT_EQ(result, VK_SUCCESS);

    std::vector<std::uint8_t> read_data;
    result = deviceToHost(handles.device, staging_buffer_download->device_memory, 0u, buffer_size, read_data);
    EXPECT_TRUE(result);

    EXPECT_EQ(read_data.size(), original_data.size());
    for (std::size_t i = 0u; i < original_data.size(); i++)
    {
        EXPECT_EQ(read_data[i], original_data[i]) << "Mismatch at index " << i;
    }

    destroyResource(handles.device, *staging_buffer_download);
    vkFreeMemory(handles.device, image_memory, nullptr);
    vkDestroyImage(handles.device, image, nullptr);
    destroyResource(handles.device, *staging_buffer_upload);

    terminateVulkan(handles);
    vulkan_setup.terminate();
}
