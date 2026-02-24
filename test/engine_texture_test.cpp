#include <algorithm>

#include <gtest/gtest.h>

#include "core/core.h"
#include "engine/engine.h"
#include "gpu/gpu.h"

namespace
{

struct VulkanHandles
{
    VkInstance instance{ VK_NULL_HANDLE };
    VkPhysicalDevice physical_device{ VK_NULL_HANDLE };
    std::uint32_t queue_family_index{ 0u };
    VkDevice device{ VK_NULL_HANDLE };
    VkQueue queue{ VK_NULL_HANDLE };
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

    queue_family_indices = QueueFamilyIndexFlagsFilter{ VK_QUEUE_GRAPHICS_BIT, queue_family_properties } << queue_family_indices;
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

} // namespace

TEST(TestTextureCube, CreateSingleMip)
{
    VulkanSetup vulkan_setup{};
    ASSERT_TRUE(vulkan_setup.init());

    VulkanHandles handles{};
    ASSERT_TRUE(initVulkan(handles));

    TextureCube cube(handles.physical_device, handles.device);
    cube.setSize(64u);
    cube.setFormat(VK_FORMAT_R32G32B32A32_SFLOAT);
    cube.setUsage(VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    ASSERT_TRUE(cube.create());
    EXPECT_TRUE(cube.isValid());
    EXPECT_NE(cube.getImage(), VK_NULL_HANDLE);
    EXPECT_NE(cube.getImageView(), VK_NULL_HANDLE);

    // Single-mip: mip 0 storage view must be valid
    EXPECT_NE(cube.getStorageImageView(), VK_NULL_HANDLE);
    EXPECT_NE(cube.getStorageImageViewForMip(0u), VK_NULL_HANDLE);

    // Convenience accessor must match mip 0
    EXPECT_EQ(cube.getStorageImageView(), cube.getStorageImageViewForMip(0u));

    // Out-of-bounds must return null
    EXPECT_EQ(cube.getStorageImageViewForMip(1u), VK_NULL_HANDLE);

    cube.destroy();
    terminateVulkan(handles);
    vulkan_setup.terminate();
}

TEST(TestTextureCube, CreateMultiMip)
{
    VulkanSetup vulkan_setup{};
    ASSERT_TRUE(vulkan_setup.init());

    VulkanHandles handles{};
    ASSERT_TRUE(initVulkan(handles));

    constexpr uint32_t MIP_LEVELS{ 5u };

    TextureCube cube(handles.physical_device, handles.device);
    cube.setSize(128u);
    cube.setFormat(VK_FORMAT_R32G32B32A32_SFLOAT);
    cube.setMipLevels(MIP_LEVELS);
    cube.setUsage(VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    ASSERT_TRUE(cube.create());
    EXPECT_TRUE(cube.isValid());
    EXPECT_EQ(cube.getMipLevels(), MIP_LEVELS);

    // Every mip level must have its own valid storage view
    for (uint32_t mip = 0u; mip < MIP_LEVELS; ++mip)
    {
        EXPECT_NE(cube.getStorageImageViewForMip(mip), VK_NULL_HANDLE) << "Mip " << mip << " storage view is NULL";
    }

    // Each per-mip view must be distinct
    for (uint32_t mip = 1u; mip < MIP_LEVELS; ++mip)
    {
        EXPECT_NE(cube.getStorageImageViewForMip(mip), cube.getStorageImageViewForMip(0u)) << "Mip " << mip << " shares view with mip 0";
    }

    // Out-of-bounds must return null
    EXPECT_EQ(cube.getStorageImageViewForMip(MIP_LEVELS), VK_NULL_HANDLE);

    cube.destroy();
    terminateVulkan(handles);
    vulkan_setup.terminate();
}

TEST(TestTexture2D, CreateOnly)
{
    VulkanSetup vulkan_setup{};
    auto result = vulkan_setup.init();
    ASSERT_TRUE(result);

    VulkanHandles handles{};
    result = initVulkan(handles);
    ASSERT_TRUE(result);

    Texture2D texture(handles.physical_device, handles.device);
    texture.setExtent(256u, 256u);
    texture.setFormat(VK_FORMAT_R8G8B8A8_UNORM);
    texture.setUsage(VK_IMAGE_USAGE_SAMPLED_BIT);

    result = texture.create();
    EXPECT_TRUE(result) << "Texture2D::create() failed";
    EXPECT_TRUE(texture.isValid()) << "Texture should be valid";
    EXPECT_NE(texture.getImage(), VK_NULL_HANDLE) << "Image is NULL";
    EXPECT_NE(texture.getImageView(), VK_NULL_HANDLE) << "ImageView is NULL";

    texture.destroy();
    terminateVulkan(handles);
    vulkan_setup.terminate();
}

TEST(TestTexture2D, UploadImageData)
{
    VulkanSetup vulkan_setup{};

    auto result = vulkan_setup.init();
    ASSERT_TRUE(result) << "Failed to initialize VulkanSetup";

    VulkanHandles handles{};

    result = initVulkan(handles);
    ASSERT_TRUE(result) << "Failed to initialize Vulkan handles";
    ASSERT_NE(handles.device, VK_NULL_HANDLE) << "Device is NULL";
    ASSERT_NE(handles.physical_device, VK_NULL_HANDLE) << "Physical device is NULL";
    ASSERT_NE(handles.queue, VK_NULL_HANDLE) << "Queue is NULL";
    ASSERT_NE(handles.command_pool, VK_NULL_HANDLE) << "Command pool is NULL";

    // Test VulkanImageFactory directly first
    const std::uint32_t width = 256u;
    const std::uint32_t height = 256u;
    const VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;

    VulkanImageFactory image_factory{ handles.device, format, { width, height, 1u }, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT };
    VkImage test_image = image_factory.create();
    ASSERT_NE(test_image, VK_NULL_HANDLE) << "VulkanImageFactory failed to create image";

    VkDeviceMemory test_memory = buildImageDeviceMemory(handles.physical_device, handles.device, test_image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    ASSERT_NE(test_memory, VK_NULL_HANDLE) << "buildImageDeviceMemory failed";

    // Test image view creation
    VkImageSubresourceRange subresource_range{};
    subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource_range.baseMipLevel = 0u;
    subresource_range.levelCount = 1u;
    subresource_range.baseArrayLayer = 0u;
    subresource_range.layerCount = 1u;

    VulkanImageViewFactory image_view_factory{ handles.device, test_image, VK_IMAGE_VIEW_TYPE_2D, format, subresource_range };
    VkImageView test_image_view = image_view_factory.create();
    ASSERT_NE(test_image_view, VK_NULL_HANDLE) << "VulkanImageViewFactory failed to create image view";

    // Cleanup test resources
    vkDestroyImageView(handles.device, test_image_view, nullptr);
    vkFreeMemory(handles.device, test_memory, nullptr);
    vkDestroyImage(handles.device, test_image, nullptr);

    // Create test image data
    ImageData image_data{};
    image_data.width = width;
    image_data.height = height;
    image_data.channels = 4u;
    image_data.channel_format = ChannelFormat::UNORM;
    image_data.color_space = ColorSpace::SRGB;
    image_data.linear = false;

    // Create test pattern
    image_data.pixels.resize(width * height * 4u);
    for (std::uint32_t y = 0u; y < height; y++)
    {
        for (std::uint32_t x = 0u; x < width; x++)
        {
            std::size_t index = (y * width + x) * 4u;
            image_data.pixels[index + 0] = static_cast<std::uint8_t>(x % 256);
            image_data.pixels[index + 1] = static_cast<std::uint8_t>(y % 256);
            image_data.pixels[index + 2] = 128u;
            image_data.pixels[index + 3] = 255u;
        }
    }

    // Create Texture2D
    Texture2D texture(handles.physical_device, handles.device);

    texture.setExtent(width, height);
    EXPECT_EQ(texture.getWidth(), width) << "Width not set correctly";
    EXPECT_EQ(texture.getHeight(), height) << "Height not set correctly";

    texture.setFormat(VK_FORMAT_R8G8B8A8_SRGB);
    EXPECT_EQ(texture.getFormat(), VK_FORMAT_R8G8B8A8_SRGB) << "Format not set correctly";

    texture.setUsage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    EXPECT_NE(texture.getUsage(), 0u) << "Usage not set correctly";

    result = texture.create();
    ASSERT_TRUE(result) << "Failed to create Texture2D - check VulkanImageFactory or memory allocation";

    EXPECT_TRUE(texture.isValid()) << "Texture should be valid after creation";
    EXPECT_NE(texture.getImage(), VK_NULL_HANDLE) << "VkImage should not be NULL";

    // Debug checks before upload
    EXPECT_EQ(image_data.width, texture.getWidth()) << "ImageData width mismatch";
    EXPECT_EQ(image_data.height, texture.getHeight()) << "ImageData height mismatch";

    VkFormat expected_format = getVulkanFormat(image_data);
    EXPECT_EQ(expected_format, texture.getFormat()) << "Format mismatch: ImageData format=" << expected_format << ", Texture format=" << texture.getFormat();

    // Upload image data
    result = texture.upload(handles.command_pool, handles.queue, image_data);
    ASSERT_TRUE(result) << "Failed to upload image data to Texture2D";

    // Verify texture is valid
    EXPECT_TRUE(texture.isValid());
    EXPECT_NE(texture.getImage(), VK_NULL_HANDLE);
    EXPECT_NE(texture.getImageView(), VK_NULL_HANDLE);
    EXPECT_EQ(texture.getWidth(), width);
    EXPECT_EQ(texture.getHeight(), height);

    texture.destroy();

    terminateVulkan(handles);
    vulkan_setup.terminate();
}
