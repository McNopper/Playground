#include <algorithm>
#include <map>
#include <vector>

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
    uint32_t queue_family_index{ 0u };
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
    physical_devices = PhysicalDeviceTypePriorityFilter{
        VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
        VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,
        VK_PHYSICAL_DEVICE_TYPE_CPU
    } << physical_devices;

    if (physical_devices.empty())
    {
        return false;
    }
    handles.physical_device = physical_devices[0u];

    auto queue_family_properties = gatherPhysicalDeviceQueueFamilyProperties2(handles.physical_device);
    std::vector<uint32_t> queue_family_indices(queue_family_properties.size());
    std::generate(queue_family_indices.begin(), queue_family_indices.end(), [index = 0u]() mutable { return index++; });

    queue_family_indices = QueueFamilyIndexFlagsFilter{ VK_QUEUE_COMPUTE_BIT, queue_family_properties } << queue_family_indices;
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

    VulkanCommandPoolFactory command_pool_factory{ handles.device, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, handles.queue_family_index };
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

TEST(TestBRDF, GenerateLUT)
{
    constexpr uint32_t LUT_SIZE = 512;

    VulkanSetup vulkan_setup{};
    auto result = vulkan_setup.init();
    ASSERT_TRUE(result) << "Failed to initialize VulkanSetup";

    VulkanHandles handles{};
    result = initVulkan(handles);
    ASSERT_TRUE(result) << "Failed to initialize Vulkan";
    ASSERT_NE(handles.device, VK_NULL_HANDLE);
    ASSERT_NE(handles.queue, VK_NULL_HANDLE);
    ASSERT_NE(handles.command_pool, VK_NULL_HANDLE);

    // Create output texture
    Texture2D output_texture{ handles.physical_device, handles.device };
    output_texture.setExtent(LUT_SIZE, LUT_SIZE);
    output_texture.setFormat(VK_FORMAT_R32G32B32A32_SFLOAT);
    output_texture.setUsage(VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);

    result = output_texture.create();
    ASSERT_TRUE(result) << "Failed to create output texture";
    ASSERT_TRUE(output_texture.isValid());

    // Compile compute shader
    std::map<std::string, std::string> macros{};
    auto shaders = buildSlang("../resources/shaders/brdf_lut.slang", macros, "../resources/shaders/");
    ASSERT_FALSE(shaders.empty()) << "Failed to compile compute shader";
    EXPECT_GT(shaders[0].code.size(), 0u);

    // Create shader module
    VulkanShaderModuleFactory shader_factory{ handles.device, shaders[0].code };
    VkShaderModule shader_module = shader_factory.create();
    ASSERT_NE(shader_module, VK_NULL_HANDLE);

    // Get descriptor bindings from SPIR-V reflection
    VulkanSpirvQuery spirv_query{ shaders };
    auto descriptor_bindings = spirv_query.gatherDescriptorSetLayoutBindings();
    ASSERT_FALSE(descriptor_bindings.empty());

    // Create descriptor set layout using factory
    VulkanDescriptorSetLayoutFactory descriptor_set_layout_factory{ handles.device, 0 };
    for (const auto& binding : descriptor_bindings)
    {
        descriptor_set_layout_factory.addDescriptorSetLayoutBinding(binding);
    }
    VkDescriptorSetLayout descriptor_set_layout = descriptor_set_layout_factory.create();
    ASSERT_NE(descriptor_set_layout, VK_NULL_HANDLE);

    // Create pipeline layout using factory
    VulkanPipelineLayoutFactory pipeline_layout_factory{ handles.device, 0 };
    pipeline_layout_factory.addDescriptorSetLayout(descriptor_set_layout);
    VkPipelineLayout pipeline_layout = pipeline_layout_factory.create();
    ASSERT_NE(pipeline_layout, VK_NULL_HANDLE);

    // Create compute pipeline
    VkPipelineShaderStageCreateInfo stage{};
    stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    stage.module = shader_module;
    stage.pName = "main";

    VulkanComputePipelineFactory pipeline_factory{ handles.device, 0, stage, pipeline_layout };
    VkPipeline compute_pipeline = pipeline_factory.create();
    ASSERT_NE(compute_pipeline, VK_NULL_HANDLE);

    // Create descriptor pool and set
    VkDescriptorPoolSize pool_size{};
    pool_size.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    pool_size.descriptorCount = 1;

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = 1;
    pool_info.pPoolSizes = &pool_size;
    pool_info.maxSets = 1;

    VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
    VkResult vk_result = vkCreateDescriptorPool(handles.device, &pool_info, nullptr, &descriptor_pool);
    ASSERT_EQ(vk_result, VK_SUCCESS);

    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = descriptor_pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &descriptor_set_layout;

    VkDescriptorSet descriptor_set = VK_NULL_HANDLE;
    vk_result = vkAllocateDescriptorSets(handles.device, &alloc_info, &descriptor_set);
    ASSERT_EQ(vk_result, VK_SUCCESS);

    // Get command buffer
    VulkanCommandBufferFactory command_buffer_factory{ handles.device, handles.command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1 };
    auto command_buffers = command_buffer_factory.create();
    ASSERT_FALSE(command_buffers.empty());
    VkCommandBuffer cmd = command_buffers[0];

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VkCommandBufferSubmitInfo cmd_submit_info{};
    cmd_submit_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    cmd_submit_info.commandBuffer = cmd;

    VkSubmitInfo2 submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submit_info.commandBufferInfoCount = 1;
    submit_info.pCommandBufferInfos = &cmd_submit_info;

    // Transition image to GENERAL layout
    vkBeginCommandBuffer(cmd, &begin_info);
    transitionImageLayout(cmd, output_texture.getImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
    vkEndCommandBuffer(cmd);
    vkQueueSubmit2(handles.queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(handles.queue);

    // Update descriptor set
    VkDescriptorImageInfo image_info{};
    image_info.imageView = output_texture.getImageView();
    image_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkWriteDescriptorSet descriptor_write{};
    descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.dstSet = descriptor_set;
    descriptor_write.dstBinding = 0;
    descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    descriptor_write.descriptorCount = 1;
    descriptor_write.pImageInfo = &image_info;

    vkUpdateDescriptorSets(handles.device, 1, &descriptor_write, 0, nullptr);

    // Generate BRDF LUT
    vkResetCommandBuffer(cmd, 0);
    vkBeginCommandBuffer(cmd, &begin_info);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, compute_pipeline);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline_layout, 0, 1, &descriptor_set, 0, nullptr);
    vkCmdDispatch(cmd, (LUT_SIZE + 7) / 8, (LUT_SIZE + 7) / 8, 1);
    vkEndCommandBuffer(cmd);

    vkQueueSubmit2(handles.queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(handles.queue);

    // Read back using staging buffer
    auto staging_buffer_opt = createStagingBuffer(handles.physical_device, handles.device, LUT_SIZE * LUT_SIZE * 4 * sizeof(float));
    ASSERT_TRUE(staging_buffer_opt.has_value());
    if (!staging_buffer_opt.has_value())
    {
        return;
    }
    auto& staging_buffer = staging_buffer_opt.value();

    // Copy image to buffer (transition to transfer src layout first)
    vkResetCommandBuffer(cmd, 0);
    vkBeginCommandBuffer(cmd, &begin_info);
    transitionImageLayout(cmd, output_texture.getImage(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    copyImageToBuffer(cmd, output_texture.getImage(), staging_buffer.buffer, { LUT_SIZE, LUT_SIZE, 1 });
    vkEndCommandBuffer(cmd);
    vkQueueSubmit2(handles.queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(handles.queue);

    // Verify data
    void* data;
    vkMapMemory(handles.device, staging_buffer.device_memory, 0, VK_WHOLE_SIZE, 0, &data);

    float* pixels = static_cast<float*>(data);

    // Sanity checks on BRDF LUT values
    EXPECT_GT(pixels[0], 0.0f) << "Bottom-left R should be positive";
    EXPECT_GE(pixels[1], 0.0f) << "Bottom-left G should be non-negative";

    size_t top_right_idx = ((LUT_SIZE - 1) * LUT_SIZE + (LUT_SIZE - 1)) * 4;
    EXPECT_GT(pixels[top_right_idx], 0.0f) << "Top-right R should be positive";

    // All values should be in valid range
    bool all_valid = true;
    for (uint32_t i = 0; i < LUT_SIZE * LUT_SIZE && all_valid; ++i)
    {
        float r = pixels[i * 4];
        float g = pixels[i * 4 + 1];
        if (r < 0.0f || r > 1.2f || g < 0.0f || g > 1.2f)
        {
            all_valid = false;
        }
    }
    EXPECT_TRUE(all_valid) << "All BRDF values should be in valid range [0, 1.2]";

    // Save to file for visual inspection
    ImageData image_data{};
    image_data.width = LUT_SIZE;
    image_data.height = LUT_SIZE;
    image_data.channels = 4;
    image_data.channel_format = ChannelFormat::SFLOAT;
    image_data.color_space = ColorSpace::SRGB;
    image_data.linear = true;
    image_data.pixels.resize(LUT_SIZE * LUT_SIZE * 4 * sizeof(float));
    memcpy(image_data.pixels.data(), data, image_data.pixels.size());

    vkUnmapMemory(handles.device, staging_buffer.device_memory);

    result = saveImageData("brdf_lut_test.exr", image_data);
    EXPECT_TRUE(result) << "Failed to save BRDF LUT test output";

    // Cleanup
    vkFreeMemory(handles.device, staging_buffer.device_memory, nullptr);
    vkDestroyBuffer(handles.device, staging_buffer.buffer, nullptr);
    output_texture.destroy();
    vkDestroyDescriptorPool(handles.device, descriptor_pool, nullptr);
    vkDestroyPipeline(handles.device, compute_pipeline, nullptr);
    vkDestroyPipelineLayout(handles.device, pipeline_layout, nullptr);
    vkDestroyDescriptorSetLayout(handles.device, descriptor_set_layout, nullptr);
    vkDestroyShaderModule(handles.device, shader_module, nullptr);

    terminateVulkan(handles);
    vulkan_setup.terminate();
}
