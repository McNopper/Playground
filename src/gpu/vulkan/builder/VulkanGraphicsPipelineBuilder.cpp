#include "VulkanGraphicsPipelineBuilder.h"

#include <optional>


#include "gpu/vulkan/factory/VulkanShaderModuleFactory.h"

#include "gpu/vulkan/spirv/vulkan_spirv.h"
#include "gpu/vulkan/spirv/VulkanSpirvQuery.h"

VulkanGraphicsPipelineBuilder::VulkanGraphicsPipelineBuilder(VkDevice device, VkPipelineCreateFlags pipeline_create_flags, VkPipelineLayout layout) :
    m_device{ device }, m_pipeline_create_flags{ pipeline_create_flags }, m_layout{ layout }
{
}

void VulkanGraphicsPipelineBuilder::setPipelineCache(VkPipelineCache pipeline_cache)
{
    m_pipeline_cache = pipeline_cache;
}

void VulkanGraphicsPipelineBuilder::addColorAttachment(VkFormat format, const VkPipelineColorBlendAttachmentState& state)
{
    m_color_attachment_formats.push_back(format);

    m_pipeline_color_blend_attachment_states.push_back(state);
}

void VulkanGraphicsPipelineBuilder::setDepthAttachment(VkFormat format)
{
    m_depth_attachment_format = format;
}

void VulkanGraphicsPipelineBuilder::setStencilAttachment(VkFormat format)
{
    m_stencil_attachment_format = format;
}

void VulkanGraphicsPipelineBuilder::setRasterizationSamples(VkSampleCountFlagBits rasterization_samples)
{
    m_rasterization_samples = rasterization_samples;
}

void VulkanGraphicsPipelineBuilder::setDepthTestEnable(VkBool32 enable)
{
    m_depth_test_enable = enable;
}

void VulkanGraphicsPipelineBuilder::setDepthWriteEnable(VkBool32 enable)
{
    m_depth_write_enable = enable;
}

void VulkanGraphicsPipelineBuilder::setDepthCompareOp(VkCompareOp compare_op)
{
    m_depth_compare_op = compare_op;
}

void VulkanGraphicsPipelineBuilder::addDynamicState(VkDynamicState dynamic_state)
{
    m_dynamic_states.push_back(dynamic_state);
}

VkPipeline VulkanGraphicsPipelineBuilder::create(const std::vector<SpirvData>& shaders) const
{
    VulkanSpirvQuery spirv_query{ shaders };

    std::optional<VkVertexInputBindingDescription> vertex_input_binding_description = spirv_query.gatherVertexInputBindingDescription(m_vertex_input_binding);
    if (!vertex_input_binding_description.has_value())
    {
        return {};
    }

    std::vector<VkVertexInputAttributeDescription> vertex_input_attribute_descriptions = spirv_query.gatherVertexInputAttributeDescriptions(m_vertex_input_binding);
    if (vertex_input_attribute_descriptions.empty())
    {
        return {};
    }

    //

    std::vector<VkPipelineShaderStageCreateInfo> pipeline_shader_stage_create_infos(shaders.size());
    for (uint32_t i = 0u; i < (uint32_t)shaders.size(); i++)
    {
        pipeline_shader_stage_create_infos[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pipeline_shader_stage_create_infos[i].pName = "main";

        VulkanShaderModuleFactory shader_module_factory{ m_device, shaders[i].code };
        pipeline_shader_stage_create_infos[i].module = shader_module_factory.create();

        auto stage = toVulkanShaderStage(shaders[i].execution_model);
        if (pipeline_shader_stage_create_infos[i].module == VK_NULL_HANDLE || !stage.has_value())
        {
            for (auto& create_info : pipeline_shader_stage_create_infos)
            {
                if (create_info.module != VK_NULL_HANDLE)
                {
                    vkDestroyShaderModule(m_device, create_info.module, nullptr);
                }
            }

            return VK_NULL_HANDLE;
        }
        pipeline_shader_stage_create_infos[i].stage = *stage;
    }

    //

    VkPipelineRenderingCreateInfo pipeline_rendering_create_info{ VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
    pipeline_rendering_create_info.viewMask = m_view_mask;
    pipeline_rendering_create_info.colorAttachmentCount = (uint32_t)m_color_attachment_formats.size();
    if (!m_color_attachment_formats.empty())
    {
        pipeline_rendering_create_info.pColorAttachmentFormats = m_color_attachment_formats.data();
    }
    pipeline_rendering_create_info.depthAttachmentFormat = m_depth_attachment_format;
    pipeline_rendering_create_info.stencilAttachmentFormat = m_stencil_attachment_format;

    //

    VkGraphicsPipelineCreateInfo graphics_pipeline_create_info{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
    graphics_pipeline_create_info.pNext = &pipeline_rendering_create_info;
    graphics_pipeline_create_info.flags = m_pipeline_create_flags;
    graphics_pipeline_create_info.stageCount = (uint32_t)pipeline_shader_stage_create_infos.size();
    if (!pipeline_shader_stage_create_infos.empty())
    {
        graphics_pipeline_create_info.pStages = pipeline_shader_stage_create_infos.data();
    }

    //

    VkPipelineVertexInputStateCreateInfo pipeline_vertex_input_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
    pipeline_vertex_input_state_create_info.vertexBindingDescriptionCount = 1u;
    pipeline_vertex_input_state_create_info.pVertexBindingDescriptions = &(*vertex_input_binding_description);
    pipeline_vertex_input_state_create_info.vertexAttributeDescriptionCount = (uint32_t)vertex_input_attribute_descriptions.size();
    pipeline_vertex_input_state_create_info.pVertexAttributeDescriptions = vertex_input_attribute_descriptions.data();

    graphics_pipeline_create_info.pVertexInputState = &pipeline_vertex_input_state_create_info;

    //

    VkPipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
    pipeline_input_assembly_state_create_info.flags = m_pipeline_input_assembly_state_create_flags;
    pipeline_input_assembly_state_create_info.topology = m_topology;
    pipeline_input_assembly_state_create_info.primitiveRestartEnable = m_primitive_restart_enable;

    graphics_pipeline_create_info.pInputAssemblyState = &pipeline_input_assembly_state_create_info;

    //

    VkPipelineTessellationStateCreateInfo pipeline_tessellation_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO };

    if (m_patch_control_points > 0u)
    {
        pipeline_tessellation_state_create_info.flags = m_pipeline_tessellation_state_create_flags;
        pipeline_tessellation_state_create_info.patchControlPoints = m_patch_control_points;

        graphics_pipeline_create_info.pTessellationState = &pipeline_tessellation_state_create_info;
    }

    //

    VkPipelineViewportStateCreateInfo pipeline_viewport_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
    pipeline_viewport_state_create_info.flags = m_pipeline_viewport_state_create_flags;

    pipeline_viewport_state_create_info.viewportCount = 1u;
    pipeline_viewport_state_create_info.scissorCount = 1u;

    graphics_pipeline_create_info.pViewportState = &pipeline_viewport_state_create_info;

    //

    VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };

    pipeline_rasterization_state_create_info.flags = m_pipeline_rasterization_state_create_flags;
    pipeline_rasterization_state_create_info.depthClampEnable = m_depth_clamp_enable;
    pipeline_rasterization_state_create_info.rasterizerDiscardEnable = m_rasterizer_discard_enable;
    pipeline_rasterization_state_create_info.polygonMode = m_polygon_mode;
    pipeline_rasterization_state_create_info.cullMode = m_cull_mode;
    pipeline_rasterization_state_create_info.frontFace = m_front_face;
    pipeline_rasterization_state_create_info.depthBiasEnable = m_depth_bias_enable;
    pipeline_rasterization_state_create_info.depthBiasConstantFactor = m_depth_bias_constant_factor;
    pipeline_rasterization_state_create_info.depthBiasClamp = m_depth_bias_clamp;
    pipeline_rasterization_state_create_info.depthBiasSlopeFactor = m_depth_bias_slope_factor;
    pipeline_rasterization_state_create_info.lineWidth = m_line_width;

    graphics_pipeline_create_info.pRasterizationState = &pipeline_rasterization_state_create_info;

    //

    VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };

    pipeline_multisample_state_create_info.flags = m_pipeline_multisample_state_create_flags;
    pipeline_multisample_state_create_info.rasterizationSamples = m_rasterization_samples;
    pipeline_multisample_state_create_info.sampleShadingEnable = m_sample_shading_enable;
    pipeline_multisample_state_create_info.minSampleShading = m_min_sample_shading;
    pipeline_multisample_state_create_info.pSampleMask = nullptr;
    if (!m_sample_mask.empty())
    {
        pipeline_multisample_state_create_info.pSampleMask = m_sample_mask.data();
    }
    pipeline_multisample_state_create_info.alphaToCoverageEnable = m_alpha_to_coverage_enable;
    pipeline_multisample_state_create_info.alphaToOneEnable = m_alpha_to_one_enable;

    graphics_pipeline_create_info.pMultisampleState = &pipeline_multisample_state_create_info;

    //

    VkPipelineDepthStencilStateCreateInfo pipeline_depth_stencil_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };

    if (m_depth_attachment_format != VK_FORMAT_UNDEFINED || m_stencil_attachment_format != VK_FORMAT_UNDEFINED)
    {
        pipeline_depth_stencil_state_create_info.flags = m_pipeline_depth_stencil_state_create_flags;
        pipeline_depth_stencil_state_create_info.depthTestEnable = m_depth_test_enable;
        pipeline_depth_stencil_state_create_info.depthWriteEnable = m_depth_write_enable;
        pipeline_depth_stencil_state_create_info.depthCompareOp = m_depth_compare_op;
        pipeline_depth_stencil_state_create_info.depthBoundsTestEnable = m_depth_bounds_test_enable;
        pipeline_depth_stencil_state_create_info.stencilTestEnable = m_stencil_test_enable;
        pipeline_depth_stencil_state_create_info.front = m_front;
        pipeline_depth_stencil_state_create_info.back = m_back;
        pipeline_depth_stencil_state_create_info.minDepthBounds = m_min_depth_bounds;
        pipeline_depth_stencil_state_create_info.maxDepthBounds = m_max_depth_bounds;

        graphics_pipeline_create_info.pDepthStencilState = &pipeline_depth_stencil_state_create_info;
    }

    //

    VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };

    pipeline_color_blend_state_create_info.flags = m_pipeline_color_blend_state_create_flags;
    pipeline_color_blend_state_create_info.logicOpEnable = m_logic_op_enable;
    pipeline_color_blend_state_create_info.logicOp = m_logic_op;
    pipeline_color_blend_state_create_info.attachmentCount = (uint32_t)m_pipeline_color_blend_attachment_states.size();
    if (!m_pipeline_color_blend_attachment_states.empty())
    {
        pipeline_color_blend_state_create_info.pAttachments = m_pipeline_color_blend_attachment_states.data();
    }
    pipeline_color_blend_state_create_info.blendConstants[0] = m_blend_constants[0];
    pipeline_color_blend_state_create_info.blendConstants[1] = m_blend_constants[1];
    pipeline_color_blend_state_create_info.blendConstants[2] = m_blend_constants[2];
    pipeline_color_blend_state_create_info.blendConstants[3] = m_blend_constants[3];

    graphics_pipeline_create_info.pColorBlendState = &pipeline_color_blend_state_create_info;

    //

    VkPipelineDynamicStateCreateInfo pipeline_dynamic_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };

    if (!m_dynamic_states.empty())
    {
        pipeline_dynamic_state_create_info.flags = m_pipeline_dynamic_state_create_flags;
        pipeline_dynamic_state_create_info.dynamicStateCount = (uint32_t)m_dynamic_states.size();
        pipeline_dynamic_state_create_info.pDynamicStates = m_dynamic_states.data();

        graphics_pipeline_create_info.pDynamicState = &pipeline_dynamic_state_create_info;
    }

    //

    graphics_pipeline_create_info.layout = m_layout;
    graphics_pipeline_create_info.subpass = m_subpass;
    graphics_pipeline_create_info.basePipelineHandle = m_base_pipeline_handle;
    graphics_pipeline_create_info.basePipelineIndex = m_base_pipeline_index;

    VkPipeline pipeline{ VK_NULL_HANDLE };
    vkCreateGraphicsPipelines(m_device, m_pipeline_cache, 1u, &graphics_pipeline_create_info, nullptr, &pipeline);

    //

    for (auto& create_info : pipeline_shader_stage_create_infos)
    {
        if (create_info.module != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(m_device, create_info.module, nullptr);
        }
    }

    return pipeline;
}
