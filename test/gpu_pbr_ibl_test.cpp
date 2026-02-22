#include <gtest/gtest.h>

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "core/core.h"
#include "gpu/gpu.h"
#include "engine/engine.h"

namespace {

constexpr uint32_t CUBE_SIZE{ 64u };
constexpr VkFormat CUBE_FORMAT{ VK_FORMAT_R32G32B32A32_SFLOAT };

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

// Transitions all 6 layers of a cube map image
void transitionCubeArrayLayout(VkCommandBuffer cmd, VkImage image, VkImageLayout old_layout, VkImageLayout new_layout)
{
    transitionImageLayout(cmd, image, old_layout, new_layout, VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 6u);
}

// Creates a descriptor pool sized from reflected bindings
VkDescriptorPool createDescriptorPool(VkDevice device, const std::vector<VkDescriptorSetLayoutBinding>& bindings)
{
    std::map<VkDescriptorType, uint32_t> type_counts{};
    for (const auto& binding : bindings)
    {
        type_counts[binding.descriptorType] += binding.descriptorCount;
    }

    std::vector<VkDescriptorPoolSize> pool_sizes{};
    pool_sizes.reserve(type_counts.size());
    for (const auto& [type, count] : type_counts)
    {
        pool_sizes.push_back({ type, count });
    }

    VkDescriptorPoolCreateInfo pool_ci{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
    pool_ci.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    pool_ci.pPoolSizes    = pool_sizes.data();
    pool_ci.maxSets       = 1u;

    VkDescriptorPool pool{ VK_NULL_HANDLE };
    vkCreateDescriptorPool(device, &pool_ci, nullptr, &pool);
    return pool;
}

// Writes descriptors based on reflected binding types
void writeIBLDescriptors(
    VkDevice device,
    VkDescriptorSet descriptor_set,
    const std::vector<VkDescriptorSetLayoutBinding>& bindings,
    VkImageView env_map_view,
    VkSampler sampler,
    VkImageView output_view)
{
    std::vector<VkWriteDescriptorSet> writes{};
    std::vector<VkDescriptorImageInfo> image_infos(3);
    uint32_t image_info_idx{ 0u };

    for (const auto& binding : bindings)
    {
        VkWriteDescriptorSet write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        write.dstSet         = descriptor_set;
        write.dstBinding     = binding.binding;
        write.descriptorCount = 1u;
        write.descriptorType = binding.descriptorType;

        if (binding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
        {
            auto& info = image_infos[image_info_idx++];
            info.imageView   = output_view;
            info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            write.pImageInfo = &info;
            writes.push_back(write);
        }
        else if (binding.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
        {
            auto& info = image_infos[image_info_idx++];
            info.imageView   = env_map_view;
            info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            info.sampler     = sampler;
            write.pImageInfo = &info;
            writes.push_back(write);
        }
        else if (binding.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
        {
            auto& info = image_infos[image_info_idx++];
            info.imageView   = env_map_view;
            info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            write.pImageInfo = &info;
            writes.push_back(write);
        }
        else if (binding.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER)
        {
            auto& info = image_infos[image_info_idx++];
            info.sampler = sampler;
            write.pImageInfo = &info;
            writes.push_back(write);
        }
    }

    vkUpdateDescriptorSets(device, static_cast<uint32_t>(writes.size()), writes.data(), 0u, nullptr);
}

}

TEST(TestIBL, GenerateDiffuseIrradiance)
{
    VulkanSetup vulkan_setup{};
    ASSERT_TRUE(vulkan_setup.init());

    VulkanHandles handles{};
    ASSERT_TRUE(initVulkan(handles));

    // Load and prepare environment map
    auto env_image_opt = loadImageData("../resources/images/day_environment.exr");
    if (!env_image_opt)
    {
        FAIL() << "Failed to load day_environment.exr";
        return;
    }
    auto env_image = std::move(*env_image_opt);
    if (env_image.channels != 4)
    {
        auto converted = convertImageDataChannels(4, env_image);
        if (!converted)
        {
            FAIL() << "Failed to convert env map to 4 channels";
            return;
        }
        env_image = std::move(*converted);
    }

    Texture2D env_texture{ handles.physical_device, handles.device };
    env_texture.setExtent(env_image.width, env_image.height);
    env_texture.setFormat(getVulkanFormat(env_image));
    env_texture.setUsage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    ASSERT_TRUE(env_texture.create());
    ASSERT_TRUE(env_texture.upload(handles.command_pool, handles.queue, env_image));

    // Create sampler
    VulkanSamplerFactory sampler_factory{ handles.device, 0u };
    sampler_factory.setMagFilter(VK_FILTER_LINEAR);
    sampler_factory.setMinFilter(VK_FILTER_LINEAR);
    sampler_factory.setSamplerAddressModeU(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
    sampler_factory.setSamplerAddressModeV(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
    VkSampler sampler = sampler_factory.create();
    ASSERT_NE(sampler, VK_NULL_HANDLE);

    // Create output cube map
    TextureCube output{ handles.physical_device, handles.device };
    output.setSize(CUBE_SIZE);
    output.setFormat(CUBE_FORMAT);
    output.setUsage(VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    ASSERT_TRUE(output.create());

    // Compile shader
    std::map<std::string, std::string> macros{};
    auto shaders = buildSlang("../resources/shaders/ibl_diffuse.slang", macros, "../resources/shaders/");
    ASSERT_FALSE(shaders.empty()) << "Failed to compile ibl_diffuse.slang";

    VulkanShaderModuleFactory shader_factory{ handles.device, shaders[0].code };
    VkShaderModule shader_module = shader_factory.create();
    ASSERT_NE(shader_module, VK_NULL_HANDLE);

    // Reflect descriptors
    VulkanSpirvQuery spirv_query{ shaders };
    auto bindings = spirv_query.gatherDescriptorSetLayoutBindings();
    ASSERT_FALSE(bindings.empty());

    VulkanDescriptorSetLayoutFactory dsl_factory{ handles.device, 0u };
    for (const auto& binding : bindings)
    {
        dsl_factory.addDescriptorSetLayoutBinding(binding);
    }
    VkDescriptorSetLayout descriptor_set_layout = dsl_factory.create();
    ASSERT_NE(descriptor_set_layout, VK_NULL_HANDLE);

    VulkanPipelineLayoutFactory pl_factory{ handles.device, 0u };
    pl_factory.addDescriptorSetLayout(descriptor_set_layout);
    VkPipelineLayout pipeline_layout = pl_factory.create();
    ASSERT_NE(pipeline_layout, VK_NULL_HANDLE);

    VkPipelineShaderStageCreateInfo stage{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    stage.stage  = VK_SHADER_STAGE_COMPUTE_BIT;
    stage.module = shader_module;
    stage.pName  = "main";

    VulkanComputePipelineFactory pipeline_factory{ handles.device, 0u, stage, pipeline_layout };
    VkPipeline pipeline = pipeline_factory.create();
    ASSERT_NE(pipeline, VK_NULL_HANDLE);

    VkDescriptorPool descriptor_pool = createDescriptorPool(handles.device, bindings);
    ASSERT_NE(descriptor_pool, VK_NULL_HANDLE);

    VkDescriptorSetAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
    alloc_info.descriptorPool     = descriptor_pool;
    alloc_info.descriptorSetCount = 1u;
    alloc_info.pSetLayouts        = &descriptor_set_layout;

    VkDescriptorSet descriptor_set{ VK_NULL_HANDLE };
    ASSERT_EQ(vkAllocateDescriptorSets(handles.device, &alloc_info, &descriptor_set), VK_SUCCESS);

    writeIBLDescriptors(handles.device, descriptor_set, bindings,
                        env_texture.getImageView(), sampler, output.getStorageImageView());

    // Record and submit compute work
    VulkanCommandBufferFactory cmd_factory{ handles.device, handles.command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1u };
    auto cmd_buffers = cmd_factory.create();
    ASSERT_FALSE(cmd_buffers.empty());
    VkCommandBuffer cmd = cmd_buffers[0];

    VkCommandBufferBeginInfo begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VkCommandBufferSubmitInfo cmd_submit{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
    cmd_submit.commandBuffer = cmd;

    VkSubmitInfo2 submit{ VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
    submit.commandBufferInfoCount = 1u;
    submit.pCommandBufferInfos    = &cmd_submit;

    vkBeginCommandBuffer(cmd, &begin_info);
    transitionCubeArrayLayout(cmd, output.getImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
    vkEndCommandBuffer(cmd);
    vkQueueSubmit2(handles.queue, 1u, &submit, VK_NULL_HANDLE);
    vkQueueWaitIdle(handles.queue);

    vkResetCommandBuffer(cmd, 0u);
    vkBeginCommandBuffer(cmd, &begin_info);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline_layout, 0u, 1u, &descriptor_set, 0u, nullptr);
    vkCmdDispatch(cmd, (CUBE_SIZE + 7u) / 8u, (CUBE_SIZE + 7u) / 8u, 6u);
    vkEndCommandBuffer(cmd);
    vkQueueSubmit2(handles.queue, 1u, &submit, VK_NULL_HANDLE);
    vkQueueWaitIdle(handles.queue);

    // Read back face 0 via staging buffer
    constexpr VkDeviceSize face_bytes{ CUBE_SIZE * CUBE_SIZE * 4u * sizeof(float) };
    auto staging_opt = createStagingBuffer(handles.physical_device, handles.device, face_bytes);
    if (!staging_opt)
    {
        FAIL() << "Failed to create staging buffer";
        return;
    }
    auto& staging = *staging_opt;

    VkImageSubresourceLayers face0_layer{};
    face0_layer.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    face0_layer.mipLevel       = 0u;
    face0_layer.baseArrayLayer = 0u;
    face0_layer.layerCount     = 1u;

    vkResetCommandBuffer(cmd, 0u);
    vkBeginCommandBuffer(cmd, &begin_info);
    transitionCubeArrayLayout(cmd, output.getImage(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    copyImageToBuffer(cmd, output.getImage(), staging.buffer, { CUBE_SIZE, CUBE_SIZE, 1u }, face0_layer);
    vkEndCommandBuffer(cmd);
    vkQueueSubmit2(handles.queue, 1u, &submit, VK_NULL_HANDLE);
    vkQueueWaitIdle(handles.queue);

    // Validate output
    void* data{ nullptr };
    vkMapMemory(handles.device, staging.device_memory, 0u, VK_WHOLE_SIZE, 0u, &data);
    const float* pixels = static_cast<const float*>(data);

    bool all_positive{ true };
    bool all_finite{ true };
    for (uint32_t i = 0u; i < CUBE_SIZE * CUBE_SIZE; ++i)
    {
        float r = pixels[i * 4u + 0u];
        float g = pixels[i * 4u + 1u];
        float b = pixels[i * 4u + 2u];
        if (r < 0.0f || g < 0.0f || b < 0.0f) { all_positive = false; }
        if (!std::isfinite(r) || !std::isfinite(g) || !std::isfinite(b)) { all_finite = false; }
    }
    EXPECT_TRUE(all_positive) << "Irradiance values should be non-negative";
    EXPECT_TRUE(all_finite)   << "Irradiance values should be finite";
    EXPECT_GT(pixels[0], 0.0f) << "Face 0 top-left should have positive irradiance";

    // Save for visual inspection
    ImageData out_image{};
    out_image.width          = CUBE_SIZE;
    out_image.height         = CUBE_SIZE;
    out_image.channels       = 4u;
    out_image.channel_format = ChannelFormat::SFLOAT;
    out_image.color_space    = ColorSpace::SRGB;
    out_image.linear         = true;
    out_image.pixels.resize(face_bytes);
    std::memcpy(out_image.pixels.data(), data, face_bytes);
    vkUnmapMemory(handles.device, staging.device_memory);

    EXPECT_TRUE(saveImageData("ibl_diffuse_face0.exr", out_image));

    // Cleanup
    vkFreeMemory(handles.device, staging.device_memory, nullptr);
    vkDestroyBuffer(handles.device, staging.buffer, nullptr);
    vkDestroyDescriptorPool(handles.device, descriptor_pool, nullptr);
    vkDestroyPipeline(handles.device, pipeline, nullptr);
    vkDestroyPipelineLayout(handles.device, pipeline_layout, nullptr);
    vkDestroyDescriptorSetLayout(handles.device, descriptor_set_layout, nullptr);
    vkDestroyShaderModule(handles.device, shader_module, nullptr);
    output.destroy();
    vkDestroySampler(handles.device, sampler, nullptr);
    env_texture.destroy();
    terminateVulkan(handles);
    vulkan_setup.terminate();
}

TEST(TestIBL, GenerateSpecularPrefilter)
{
    VulkanSetup vulkan_setup{};
    ASSERT_TRUE(vulkan_setup.init());

    VulkanHandles handles{};
    ASSERT_TRUE(initVulkan(handles));

    // Load and prepare environment map
    auto env_image_opt = loadImageData("../resources/images/day_environment.exr");
    if (!env_image_opt)
    {
        FAIL() << "Failed to load day_environment.exr";
        return;
    }
    auto env_image = std::move(*env_image_opt);
    if (env_image.channels != 4)
    {
        auto converted = convertImageDataChannels(4, env_image);
        if (!converted)
        {
            FAIL() << "Failed to convert env map to 4 channels";
            return;
        }
        env_image = std::move(*converted);
    }

    Texture2D env_texture{ handles.physical_device, handles.device };
    env_texture.setExtent(env_image.width, env_image.height);
    env_texture.setFormat(getVulkanFormat(env_image));
    env_texture.setUsage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    ASSERT_TRUE(env_texture.create());
    ASSERT_TRUE(env_texture.upload(handles.command_pool, handles.queue, env_image));

    // Create sampler
    VulkanSamplerFactory sampler_factory{ handles.device, 0u };
    sampler_factory.setMagFilter(VK_FILTER_LINEAR);
    sampler_factory.setMinFilter(VK_FILTER_LINEAR);
    sampler_factory.setSamplerAddressModeU(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
    sampler_factory.setSamplerAddressModeV(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
    VkSampler sampler = sampler_factory.create();
    ASSERT_NE(sampler, VK_NULL_HANDLE);

    // Create output cube map
    TextureCube output{ handles.physical_device, handles.device };
    output.setSize(CUBE_SIZE);
    output.setFormat(CUBE_FORMAT);
    output.setUsage(VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    ASSERT_TRUE(output.create());

    // Compile shader
    std::map<std::string, std::string> macros{};
    auto shaders = buildSlang("../resources/shaders/ibl_specular.slang", macros, "../resources/shaders/");
    ASSERT_FALSE(shaders.empty()) << "Failed to compile ibl_specular.slang";

    VulkanShaderModuleFactory shader_factory{ handles.device, shaders[0].code };
    VkShaderModule shader_module = shader_factory.create();
    ASSERT_NE(shader_module, VK_NULL_HANDLE);

    // Reflect descriptors and push constants
    VulkanSpirvQuery spirv_query{ shaders };
    auto bindings = spirv_query.gatherDescriptorSetLayoutBindings();
    ASSERT_FALSE(bindings.empty());

    auto push_constant_ranges = spirv_query.gatherPushConstantRanges();
    ASSERT_FALSE(push_constant_ranges.empty()) << "Expected push constants for roughness/sample_count";

    VulkanDescriptorSetLayoutFactory dsl_factory{ handles.device, 0u };
    for (const auto& binding : bindings)
    {
        dsl_factory.addDescriptorSetLayoutBinding(binding);
    }
    VkDescriptorSetLayout descriptor_set_layout = dsl_factory.create();
    ASSERT_NE(descriptor_set_layout, VK_NULL_HANDLE);

    VulkanPipelineLayoutFactory pl_factory{ handles.device, 0u };
    pl_factory.addDescriptorSetLayout(descriptor_set_layout);
    for (const auto& range : push_constant_ranges)
    {
        pl_factory.addPushConstantRange(range);
    }
    VkPipelineLayout pipeline_layout = pl_factory.create();
    ASSERT_NE(pipeline_layout, VK_NULL_HANDLE);

    VkPipelineShaderStageCreateInfo stage{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    stage.stage  = VK_SHADER_STAGE_COMPUTE_BIT;
    stage.module = shader_module;
    stage.pName  = "main";

    VulkanComputePipelineFactory pipeline_factory{ handles.device, 0u, stage, pipeline_layout };
    VkPipeline pipeline = pipeline_factory.create();
    ASSERT_NE(pipeline, VK_NULL_HANDLE);

    VkDescriptorPool descriptor_pool = createDescriptorPool(handles.device, bindings);
    ASSERT_NE(descriptor_pool, VK_NULL_HANDLE);

    VkDescriptorSetAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
    alloc_info.descriptorPool     = descriptor_pool;
    alloc_info.descriptorSetCount = 1u;
    alloc_info.pSetLayouts        = &descriptor_set_layout;

    VkDescriptorSet descriptor_set{ VK_NULL_HANDLE };
    ASSERT_EQ(vkAllocateDescriptorSets(handles.device, &alloc_info, &descriptor_set), VK_SUCCESS);

    writeIBLDescriptors(handles.device, descriptor_set, bindings,
                        env_texture.getImageView(), sampler, output.getStorageImageView());

    // Push constants: roughness=0.5, 512 samples
    struct PushConstants { float roughness; uint32_t sample_count; };
    PushConstants push{ 0.5f, 512u };

    // Record and submit compute work
    VulkanCommandBufferFactory cmd_factory{ handles.device, handles.command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1u };
    auto cmd_buffers = cmd_factory.create();
    ASSERT_FALSE(cmd_buffers.empty());
    VkCommandBuffer cmd = cmd_buffers[0];

    VkCommandBufferBeginInfo begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VkCommandBufferSubmitInfo cmd_submit{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
    cmd_submit.commandBuffer = cmd;

    VkSubmitInfo2 submit{ VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
    submit.commandBufferInfoCount = 1u;
    submit.pCommandBufferInfos    = &cmd_submit;

    vkBeginCommandBuffer(cmd, &begin_info);
    transitionCubeArrayLayout(cmd, output.getImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
    vkEndCommandBuffer(cmd);
    vkQueueSubmit2(handles.queue, 1u, &submit, VK_NULL_HANDLE);
    vkQueueWaitIdle(handles.queue);

    vkResetCommandBuffer(cmd, 0u);
    vkBeginCommandBuffer(cmd, &begin_info);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline_layout, 0u, 1u, &descriptor_set, 0u, nullptr);
    vkCmdPushConstants(cmd, pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0u, sizeof(PushConstants), &push);
    vkCmdDispatch(cmd, (CUBE_SIZE + 7u) / 8u, (CUBE_SIZE + 7u) / 8u, 6u);
    vkEndCommandBuffer(cmd);
    vkQueueSubmit2(handles.queue, 1u, &submit, VK_NULL_HANDLE);
    vkQueueWaitIdle(handles.queue);

    // Read back face 0 via staging buffer
    constexpr VkDeviceSize face_bytes{ CUBE_SIZE * CUBE_SIZE * 4u * sizeof(float) };
    auto staging_opt = createStagingBuffer(handles.physical_device, handles.device, face_bytes);
    if (!staging_opt)
    {
        FAIL() << "Failed to create staging buffer";
        return;
    }
    auto& staging = *staging_opt;

    VkImageSubresourceLayers face0_layer{};
    face0_layer.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    face0_layer.mipLevel       = 0u;
    face0_layer.baseArrayLayer = 0u;
    face0_layer.layerCount     = 1u;

    vkResetCommandBuffer(cmd, 0u);
    vkBeginCommandBuffer(cmd, &begin_info);
    transitionCubeArrayLayout(cmd, output.getImage(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    copyImageToBuffer(cmd, output.getImage(), staging.buffer, { CUBE_SIZE, CUBE_SIZE, 1u }, face0_layer);
    vkEndCommandBuffer(cmd);
    vkQueueSubmit2(handles.queue, 1u, &submit, VK_NULL_HANDLE);
    vkQueueWaitIdle(handles.queue);

    // Validate output
    void* data{ nullptr };
    vkMapMemory(handles.device, staging.device_memory, 0u, VK_WHOLE_SIZE, 0u, &data);
    const float* pixels = static_cast<const float*>(data);

    bool all_non_negative{ true };
    bool all_finite{ true };
    for (uint32_t i = 0u; i < CUBE_SIZE * CUBE_SIZE; ++i)
    {
        float r = pixels[i * 4u + 0u];
        float g = pixels[i * 4u + 1u];
        float b = pixels[i * 4u + 2u];
        if (r < 0.0f || g < 0.0f || b < 0.0f) { all_non_negative = false; }
        if (!std::isfinite(r) || !std::isfinite(g) || !std::isfinite(b)) { all_finite = false; }
    }
    EXPECT_TRUE(all_non_negative) << "Specular prefilter values should be non-negative";
    EXPECT_TRUE(all_finite)       << "Specular prefilter values should be finite";

    // Save for visual inspection
    ImageData out_image{};
    out_image.width          = CUBE_SIZE;
    out_image.height         = CUBE_SIZE;
    out_image.channels       = 4u;
    out_image.channel_format = ChannelFormat::SFLOAT;
    out_image.color_space    = ColorSpace::SRGB;
    out_image.linear         = true;
    out_image.pixels.resize(face_bytes);
    std::memcpy(out_image.pixels.data(), data, face_bytes);
    vkUnmapMemory(handles.device, staging.device_memory);

    EXPECT_TRUE(saveImageData("ibl_specular_face0.exr", out_image));

    // Cleanup
    vkFreeMemory(handles.device, staging.device_memory, nullptr);
    vkDestroyBuffer(handles.device, staging.buffer, nullptr);
    vkDestroyDescriptorPool(handles.device, descriptor_pool, nullptr);
    vkDestroyPipeline(handles.device, pipeline, nullptr);
    vkDestroyPipelineLayout(handles.device, pipeline_layout, nullptr);
    vkDestroyDescriptorSetLayout(handles.device, descriptor_set_layout, nullptr);
    vkDestroyShaderModule(handles.device, shader_module, nullptr);
    output.destroy();
    vkDestroySampler(handles.device, sampler, nullptr);
    env_texture.destroy();
    terminateVulkan(handles);
    vulkan_setup.terminate();
}
