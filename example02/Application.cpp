#include "Application.h"

#include <map>
#include <string>

#include "core/core.h"

Application::Application(VkPhysicalDevice physical_device, VkDevice device, uint32_t queue_family_index) :
    m_physical_device{ physical_device },
    m_device{ device },
    m_queue_family_index{ queue_family_index },
    m_output_storage_buffer(physical_device, device, false, true),
    m_descriptor_buffer(physical_device, device)
{
}

bool Application::init()
{
    vkGetDeviceQueue(m_device, m_queue_family_index, 0, &m_queue);

    if (!m_output_storage_buffer.create(c_buffer_size))
    {
        return false;
    }

    if (!m_descriptor_buffer.create(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER))
    {
        return false;
    }

    VkDescriptorBufferInfo descriptor_buffer_info{};
    descriptor_buffer_info.buffer = m_output_storage_buffer.getBuffer();
    descriptor_buffer_info.offset = 0u;
    descriptor_buffer_info.range = c_buffer_size;

    if (!m_descriptor_buffer.writeBufferDescriptor(0, descriptor_buffer_info))
    {
        return false;
    }

    std::map<std::string, std::string> macros{};
    auto shaders = buildSlang("compute_grey.slang", macros, "../resources/shaders/");
    if (shaders.size() != 1u)
    {
        return false;
    }

    VkDescriptorSetLayoutCreateFlags layout_flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_DESCRIPTOR_BUFFER_BIT_EXT;
    VulkanDescriptorSetLayoutFactory descriptor_set_layout_factory{ m_device, layout_flags };
    descriptor_set_layout_factory.addDescriptorSetLayoutBinding(
        { 0u,
          VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
          1u,
          VK_SHADER_STAGE_COMPUTE_BIT,
          nullptr });

    VkDescriptorSetLayout descriptor_set_layout = descriptor_set_layout_factory.create();
    if (descriptor_set_layout == VK_NULL_HANDLE)
    {
        return false;
    }

    VulkanPipelineLayoutFactory pipeline_layout_factory{ m_device, 0u };
    pipeline_layout_factory.addDescriptorSetLayout(descriptor_set_layout);
    m_pipeline_layout = pipeline_layout_factory.create();

    if (m_pipeline_layout == VK_NULL_HANDLE)
    {
        vkDestroyDescriptorSetLayout(m_device, descriptor_set_layout, nullptr);
        return false;
    }

    VulkanShaderModuleFactory shader_module_factory{ m_device, shaders[0].code };
    VkShaderModule shader_module = shader_module_factory.create();

    auto stage = toVulkanShaderStage(shaders[0].execution_model);

    if (shader_module == VK_NULL_HANDLE || !stage.has_value())
    {
        if (shader_module != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(m_device, shader_module, nullptr);
        }

        vkDestroyDescriptorSetLayout(m_device, descriptor_set_layout, nullptr);
        return false;
    }

    VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    pipeline_shader_stage_create_info.stage = *stage;
    pipeline_shader_stage_create_info.module = shader_module;
    pipeline_shader_stage_create_info.pName = "main";

    VulkanComputePipelineFactory compute_pipeline_factory{ m_device, VK_PIPELINE_CREATE_DESCRIPTOR_BUFFER_BIT_EXT, pipeline_shader_stage_create_info, m_pipeline_layout };
    m_pipeline = compute_pipeline_factory.create();

    vkDestroyShaderModule(m_device, shader_module, nullptr);
    vkDestroyDescriptorSetLayout(m_device, descriptor_set_layout, nullptr);

    return (m_pipeline != VK_NULL_HANDLE);
}

bool Application::update(double delta_time, VkCommandBuffer command_buffer)
{
    VkCommandBufferBeginInfo command_buffer_begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    auto result = vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);
    if (result != VK_SUCCESS)
    {
        return false;
    }

    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_pipeline);

    VkDescriptorBufferBindingInfoEXT descriptor_buffer_binding_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT };
    descriptor_buffer_binding_info.address = m_descriptor_buffer.getDeviceAddress();
    descriptor_buffer_binding_info.usage = VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT;

    vkCmdBindDescriptorBuffersEXT(command_buffer, 1, &descriptor_buffer_binding_info);

    uint32_t buffer_index = 0;
    VkDeviceSize descriptor_offset = 0;
    vkCmdSetDescriptorBufferOffsetsEXT(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_pipeline_layout, 0, 1, &buffer_index, &descriptor_offset);

    vkCmdDispatch(command_buffer, c_dimension * c_dimension, 1u, 1u);

    result = vkEndCommandBuffer(command_buffer);
    if (result != VK_SUCCESS)
    {
        return false;
    }

    VkCommandBufferSubmitInfo command_buffer_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
    command_buffer_info.commandBuffer = command_buffer;

    VkSubmitInfo2 submit_info2{ VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
    submit_info2.commandBufferInfoCount = 1u;
    submit_info2.pCommandBufferInfos = &command_buffer_info;

    result = vkQueueSubmit2(m_queue, 1u, &submit_info2, VK_NULL_HANDLE);
    if (result != VK_SUCCESS)
    {
        return false;
    }

    result = vkQueueWaitIdle(m_queue);
    if (result != VK_SUCCESS)
    {
        return false;
    }

    auto staging_buffer = createStagingBuffer(m_physical_device, m_device, c_buffer_size);
    if (!staging_buffer.has_value())
    {
        return false;
    }

    result = vkResetCommandBuffer(command_buffer, 0u);
    if (result != VK_SUCCESS)
    {
        destroyResource(m_device, *staging_buffer);
        return false;
    }

    result = vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);
    if (result != VK_SUCCESS)
    {
        destroyResource(m_device, *staging_buffer);
        return false;
    }

    VkBufferCopy2 buffer_copy{ VK_STRUCTURE_TYPE_BUFFER_COPY_2 };
    buffer_copy.srcOffset = 0u;
    buffer_copy.dstOffset = 0u;
    buffer_copy.size = c_buffer_size;

    VkCopyBufferInfo2 copy_buffer_info{ VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2 };
    copy_buffer_info.srcBuffer = m_output_storage_buffer.getBuffer();
    copy_buffer_info.dstBuffer = staging_buffer->buffer;
    copy_buffer_info.regionCount = 1u;
    copy_buffer_info.pRegions = &buffer_copy;

    vkCmdCopyBuffer2(command_buffer, &copy_buffer_info);

    result = vkEndCommandBuffer(command_buffer);
    if (result != VK_SUCCESS)
    {
        destroyResource(m_device, *staging_buffer);
        return false;
    }

    result = vkQueueSubmit2(m_queue, 1u, &submit_info2, VK_NULL_HANDLE);
    if (result != VK_SUCCESS)
    {
        destroyResource(m_device, *staging_buffer);
        return false;
    }

    result = vkQueueWaitIdle(m_queue);
    if (result != VK_SUCCESS)
    {
        destroyResource(m_device, *staging_buffer);
        return false;
    }

    std::vector<uint8_t> content{};
    bool read_result = deviceToHost<uint8_t>(m_device, staging_buffer->device_memory, 0u, c_buffer_size, content);

    destroyResource(m_device, *staging_buffer);

    if (!read_result || content.empty())
    {
        return false;
    }

    ImageData image_data{};
    image_data.width = c_dimension;
    image_data.height = c_dimension;
    image_data.channels = 1u;
    image_data.channel_format = ChannelFormat::SFLOAT;
    image_data.color_space = ColorSpace::SRGB;
    image_data.linear = true;
    image_data.pixels = std::move(content);

    return saveImageData("../bin/example02_grey_square.exr", image_data);
}

void Application::terminate()
{
    vkDeviceWaitIdle(m_device);

    if (m_pipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(m_device, m_pipeline, nullptr);
        m_pipeline = VK_NULL_HANDLE;
    }

    if (m_pipeline_layout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
        m_pipeline_layout = VK_NULL_HANDLE;
    }

    m_descriptor_buffer.destroy();
    m_output_storage_buffer.destroy();

    m_queue = VK_NULL_HANDLE;
}
