#include <gtest/gtest.h>

#include <algorithm>

#include "gpu/gpu.h"
#include "engine/engine.h"

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

TEST(TestRenderer, VertexBufferRoundTrip)
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

    // Create test data
    std::vector<std::uint8_t> original_data = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 
                                                 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 };
    
    // Create VertexBuffer with readback enabled
    VertexBuffer vertex_buffer(handles.physical_device, handles.device, true, true);
    result = vertex_buffer.create(original_data);
    EXPECT_TRUE(result);
    
    if (result)
    {
        // Verify buffer is valid
        EXPECT_TRUE(vertex_buffer.isValid());
        EXPECT_NE(vertex_buffer.getBuffer(), VK_NULL_HANDLE);
        EXPECT_EQ(vertex_buffer.getDeviceSize(), original_data.size());
        
        // Read back the data using template
        std::vector<std::uint8_t> read_data;
        result = vertex_buffer.readBack(0, read_data, original_data.size());
        EXPECT_TRUE(result);
        
        // Verify the round trip - data should match
        EXPECT_EQ(read_data.size(), original_data.size());
        EXPECT_EQ(read_data, original_data);
    }

    vertex_buffer.destroy();

    terminateVulkan(handles);

    vulkan_setup.terminate();
}

TEST(TestRenderer, IndexBufferRoundTrip)
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

    // Create test data (indices)
    std::vector<std::uint8_t> original_data = { 0x00, 0x00, 0x00, 0x00,
                                                 0x01, 0x00, 0x00, 0x00,
                                                 0x02, 0x00, 0x00, 0x00,
                                                 0x00, 0x00, 0x00, 0x00,
                                                 0x02, 0x00, 0x00, 0x00,
                                                 0x03, 0x00, 0x00, 0x00 };
    
    // Create IndexBuffer with readback enabled
    IndexBuffer index_buffer(handles.physical_device, handles.device, true, true);
    result = index_buffer.create(original_data);
    EXPECT_TRUE(result);
    
    if (result)
    {
        // Verify buffer is valid
        EXPECT_TRUE(index_buffer.isValid());
        EXPECT_NE(index_buffer.getBuffer(), VK_NULL_HANDLE);
        EXPECT_EQ(index_buffer.getDeviceSize(), original_data.size());
        
        // Read back the data using template
        std::vector<std::uint8_t> read_data;
        result = index_buffer.readBack(0, read_data, original_data.size());
        EXPECT_TRUE(result);
        
        // Verify the round trip - data should match
        EXPECT_EQ(read_data.size(), original_data.size());
        EXPECT_EQ(read_data, original_data);
    }

    index_buffer.destroy();

    terminateVulkan(handles);

    vulkan_setup.terminate();
}

TEST(TestRenderer, BufferUpdateTest)
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

    // Create initial test data
    std::vector<std::uint8_t> original_data = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
    
    // Create VertexBuffer with readback enabled
    VertexBuffer vertex_buffer(handles.physical_device, handles.device, true, true);
    result = vertex_buffer.create(original_data);
    EXPECT_TRUE(result);
    
    if (result)
    {
        // Update middle portion of the buffer using template
        std::vector<std::uint8_t> update_data = { 0xAA, 0xBB };
        result = vertex_buffer.update(2, update_data);
        EXPECT_TRUE(result);
        
        // Read back the entire buffer using template
        std::vector<std::uint8_t> read_data;
        result = vertex_buffer.readBack(0, read_data, original_data.size());
        EXPECT_TRUE(result);
        
        // Verify the update worked correctly
        EXPECT_EQ(read_data.size(), original_data.size());
        EXPECT_EQ(read_data[0], 0x01);
        EXPECT_EQ(read_data[1], 0x02);
        EXPECT_EQ(read_data[2], 0xAA);
        EXPECT_EQ(read_data[3], 0xBB);
        EXPECT_EQ(read_data[4], 0x05);
        EXPECT_EQ(read_data[5], 0x06);
        EXPECT_EQ(read_data[6], 0x07);
        EXPECT_EQ(read_data[7], 0x08);
    }

    vertex_buffer.destroy();

    terminateVulkan(handles);

    vulkan_setup.terminate();
}

TEST(TestRenderer, DescriptorBufferCreation)
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

    // Create DescriptorBuffer for uniform buffers
    DescriptorBuffer descriptor_buffer(handles.physical_device, handles.device);
    result = descriptor_buffer.create(10, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    EXPECT_TRUE(result);
    
    if (result)
    {
        // Verify descriptor buffer is valid
        EXPECT_TRUE(descriptor_buffer.isValid());
        EXPECT_NE(descriptor_buffer.getBuffer(), VK_NULL_HANDLE);
        EXPECT_NE(descriptor_buffer.getDeviceAddress(), 0u);
        EXPECT_GT(descriptor_buffer.getDescriptorSize(), 0u);
        EXPECT_EQ(descriptor_buffer.getDescriptorType(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        
        // Verify total buffer size is descriptor_count * descriptor_size
        VkDeviceSize expected_size = 10 * descriptor_buffer.getDescriptorSize();
        EXPECT_EQ(descriptor_buffer.getDeviceSize(), expected_size);
    }

    descriptor_buffer.destroy();

    terminateVulkan(handles);

    vulkan_setup.terminate();
}

TEST(TestRenderer, UniformBufferUpload)
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

    // Test struct for uniform data
    struct TestUniformData {
        float values[4];
        std::uint32_t count;
        float padding[3];
    };

    TestUniformData test_data{};
    test_data.values[0] = 1.0f;
    test_data.values[1] = 2.0f;
    test_data.values[2] = 3.0f;
    test_data.values[3] = 4.0f;
    test_data.count = 42;

    // Create UniformBuffer with readback enabled
    UniformBuffer uniform_buffer(handles.physical_device, handles.device, true, true);
    result = uniform_buffer.create(test_data);
    EXPECT_TRUE(result);
    
    if (result)
    {
        // Verify buffer is valid
        EXPECT_TRUE(uniform_buffer.isValid());
        EXPECT_NE(uniform_buffer.getBuffer(), VK_NULL_HANDLE);
        EXPECT_EQ(uniform_buffer.getDeviceSize(), sizeof(TestUniformData));
        
        // Read back and verify using template
        TestUniformData read_uniform{};
        result = uniform_buffer.readBack(0, read_uniform);
        EXPECT_TRUE(result);
        EXPECT_FLOAT_EQ(read_uniform.values[0], 1.0f);
        EXPECT_FLOAT_EQ(read_uniform.values[1], 2.0f);
        EXPECT_FLOAT_EQ(read_uniform.values[2], 3.0f);
        EXPECT_FLOAT_EQ(read_uniform.values[3], 4.0f);
        EXPECT_EQ(read_uniform.count, 42u);
    }

    uniform_buffer.destroy();

    terminateVulkan(handles);

    vulkan_setup.terminate();
}

TEST(TestRenderer, UniformBufferWithDescriptor)
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

    // Create uniform buffer with data
    struct UniformData {
        float matrix[16];
        float color[4];
    };

    UniformData uniform_data{};
    for (int i = 0; i < 16; i++)
    {
        uniform_data.matrix[i] = static_cast<float>(i);
    }

    uniform_data.color[0] = 1.0f;
    uniform_data.color[1] = 0.5f;
    uniform_data.color[2] = 0.25f;
    uniform_data.color[3] = 1.0f;

    // Create uniform buffer using template
    UniformBuffer uniform_buffer(handles.physical_device, handles.device);
    result = uniform_buffer.create(uniform_data);
    EXPECT_TRUE(result);

    // Create descriptor buffer that can point to uniform buffers
    DescriptorBuffer descriptor_buffer(handles.physical_device, handles.device);
    result = descriptor_buffer.create(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    EXPECT_TRUE(result);

    if (result)
    {
        // Verify both buffers are valid
        EXPECT_TRUE(uniform_buffer.isValid());
        EXPECT_TRUE(descriptor_buffer.isValid());
        
        // In real usage, descriptor_buffer would contain metadata pointing to uniform_buffer
        EXPECT_NE(uniform_buffer.getBuffer(), VK_NULL_HANDLE);
        EXPECT_NE(descriptor_buffer.getBuffer(), VK_NULL_HANDLE);
        EXPECT_NE(descriptor_buffer.getDeviceAddress(), 0u);
        
        // They are different buffers with different purposes
        EXPECT_NE(uniform_buffer.getBuffer(), descriptor_buffer.getBuffer());
    }

    descriptor_buffer.destroy();
    uniform_buffer.destroy();

    terminateVulkan(handles);

    vulkan_setup.terminate();
}

TEST(TestRenderer, StorageBufferUpload)
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

    // Test struct for storage buffer data
    struct TestStorageData {
        float values[4];
        std::uint32_t count;
        float padding[3];
    };

    TestStorageData test_data{};
    test_data.values[0] = 10.0f;
    test_data.values[1] = 20.0f;
    test_data.values[2] = 30.0f;
    test_data.values[3] = 40.0f;
    test_data.count = 100;

    // Create StorageBuffer with readback enabled
    StorageBuffer storage_buffer(handles.physical_device, handles.device, true, true);
    result = storage_buffer.create(test_data);
    EXPECT_TRUE(result);
    
    if (result)
    {
        // Verify buffer is valid
        EXPECT_TRUE(storage_buffer.isValid());
        EXPECT_NE(storage_buffer.getBuffer(), VK_NULL_HANDLE);
        EXPECT_EQ(storage_buffer.getDeviceSize(), sizeof(TestStorageData));
        
        // Read back and verify using template
        TestStorageData read_storage{};
        result = storage_buffer.readBack(0, read_storage);
        EXPECT_TRUE(result);
        EXPECT_FLOAT_EQ(read_storage.values[0], 10.0f);
        EXPECT_FLOAT_EQ(read_storage.values[1], 20.0f);
        EXPECT_FLOAT_EQ(read_storage.values[2], 30.0f);
        EXPECT_FLOAT_EQ(read_storage.values[3], 40.0f);
        EXPECT_EQ(read_storage.count, 100u);
    }

    storage_buffer.destroy();

    terminateVulkan(handles);

    vulkan_setup.terminate();
}

TEST(TestRenderer, StorageBufferWithDescriptor)
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

    // Create storage buffer with data
    struct StorageData {
        float values[16];
        std::uint32_t indices[4];
    };

    StorageData storage_data{};
    for (int i = 0; i < 16; i++)
    {
        storage_data.values[i] = static_cast<float>(i) * 2.0f;
    }
    
    storage_data.indices[0] = 0;
    storage_data.indices[1] = 1;
    storage_data.indices[2] = 2;
    storage_data.indices[3] = 3;

    // Create storage buffer using template
    StorageBuffer storage_buffer(handles.physical_device, handles.device);
    result = storage_buffer.create(storage_data);
    EXPECT_TRUE(result);

    // Create descriptor buffer that can point to storage buffers
    DescriptorBuffer descriptor_buffer(handles.physical_device, handles.device);
    result = descriptor_buffer.create(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    EXPECT_TRUE(result);

    if (result)
    {
        // Verify both buffers are valid
        EXPECT_TRUE(storage_buffer.isValid());
        EXPECT_TRUE(descriptor_buffer.isValid());
        
        // In real usage, descriptor_buffer would contain metadata pointing to storage_buffer
        EXPECT_NE(storage_buffer.getBuffer(), VK_NULL_HANDLE);
        EXPECT_NE(descriptor_buffer.getBuffer(), VK_NULL_HANDLE);
        EXPECT_NE(descriptor_buffer.getDeviceAddress(), 0u);
        
        // They are different buffers with different purposes
        EXPECT_NE(storage_buffer.getBuffer(), descriptor_buffer.getBuffer());
    }

    descriptor_buffer.destroy();
    storage_buffer.destroy();

    terminateVulkan(handles);

    vulkan_setup.terminate();
}
