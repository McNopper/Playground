#ifndef GPU_VULKAN_BUILDER_VULKANGRAPHICSPIPELINEBUILDER_H_
#define GPU_VULKAN_BUILDER_VULKANGRAPHICSPIPELINEBUILDER_H_

#include <vector>

#include <volk.h>

#include "gpu/shader/spirv/spirv_data.h"

class VulkanGraphicsPipelineBuilder
{

private:

    VkDevice m_device{ VK_NULL_HANDLE };

    VkPipelineCache m_pipeline_cache{ VK_NULL_HANDLE };

    VkPipelineCreateFlags m_pipeline_create_flags{ 0u };
    VkPipelineLayout m_layout{ VK_NULL_HANDLE };
    uint32_t m_subpass{ 0u };
    VkPipeline m_base_pipeline_handle{ VK_NULL_HANDLE };
    int32_t m_base_pipeline_index{ -1 };

    uint32_t m_view_mask{ 0u };
    std::vector<VkFormat> m_color_attachment_formats{};
    VkFormat m_depth_attachment_format{ VK_FORMAT_UNDEFINED };
    VkFormat m_stencil_attachment_format{ VK_FORMAT_UNDEFINED };

    VkPipelineInputAssemblyStateCreateFlags m_pipeline_input_assembly_state_create_flags{ 0u };
    VkPrimitiveTopology m_topology{ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST };
    VkBool32 m_primitive_restart_enable{ VK_FALSE };

    VkPipelineTessellationStateCreateFlags m_pipeline_tessellation_state_create_flags{ 0u };
    uint32_t m_patch_control_points{ 0u };

    VkPipelineRasterizationStateCreateFlags m_pipeline_rasterization_state_create_flags{ 0u };
    VkBool32 m_depth_clamp_enable{ VK_FALSE };
    VkBool32 m_rasterizer_discard_enable{ VK_FALSE };
    VkPolygonMode m_polygon_mode{ VK_POLYGON_MODE_FILL };
    VkCullModeFlags m_cull_mode{ VK_CULL_MODE_NONE };
    VkFrontFace m_front_face{ VK_FRONT_FACE_COUNTER_CLOCKWISE };
    VkBool32 m_depth_bias_enable{ VK_FALSE };
    float m_depth_bias_constant_factor{ 0.0f };
    float m_depth_bias_clamp{ 0.0f };
    float m_depth_bias_slope_factor{ 0.0f };
    float m_line_width{ 1.0f };

    VkPipelineMultisampleStateCreateFlags m_pipeline_multisample_state_create_flags{ 0u };
    VkSampleCountFlagBits m_rasterization_samples{ VK_SAMPLE_COUNT_1_BIT };
    VkBool32 m_sample_shading_enable{ VK_FALSE };
    float m_min_sample_shading{ 0.0f };
    std::vector<VkSampleMask> m_sample_mask{};
    VkBool32 m_alpha_to_coverage_enable{ VK_FALSE };
    VkBool32 m_alpha_to_one_enable{ VK_FALSE };

    VkPipelineDepthStencilStateCreateFlags m_pipeline_depth_stencil_state_create_flags{ 0u };
    VkBool32 m_depth_test_enable{ VK_FALSE };
    VkBool32 m_depth_write_enable{ VK_FALSE };
    VkCompareOp m_depth_compare_op{ VK_COMPARE_OP_NEVER };
    VkBool32 m_depth_bounds_test_enable{ VK_FALSE };
    VkBool32 m_stencil_test_enable{ VK_FALSE };
    VkStencilOpState m_front{};
    VkStencilOpState m_back{};
    float m_min_depth_bounds{ 0.0f };
    float m_max_depth_bounds{ 0.0f };

    VkPipelineColorBlendStateCreateFlags m_pipeline_color_blend_state_create_flags{ 0u };
    VkBool32 m_logic_op_enable{ VK_FALSE };
    VkLogicOp m_logic_op{ VK_LOGIC_OP_CLEAR };
    std::vector<VkPipelineColorBlendAttachmentState> m_pipeline_color_blend_attachment_states{};
    float m_blend_constants[4]{ 0.0f, 0.0f, 0.0f, 0.0f };

    VkPipelineDynamicStateCreateFlags m_pipeline_dynamic_state_create_flags{ 0u };
    std::vector<VkDynamicState> m_dynamic_states{};

    VkPipelineViewportStateCreateFlags m_pipeline_viewport_state_create_flags{ 0u };
    std::vector<VkViewport> m_viewports{};
    std::vector<VkRect2D> m_scissors{};

    uint32_t m_vertex_input_binding{ 0u };

public:

    VulkanGraphicsPipelineBuilder() = delete;

    VulkanGraphicsPipelineBuilder(VkDevice device, VkPipelineCreateFlags pipeline_create_flags, VkPipelineLayout layout);

    void setPipelineCache(VkPipelineCache pipeline_cache);

    void addColorAttachment(VkFormat format, const VkPipelineColorBlendAttachmentState& state);
    void setDepthAttachment(VkFormat format);
    void setStencilAttachment(VkFormat format);

    void setRasterizationSamples(VkSampleCountFlagBits rasterization_samples);

    void setDepthTestEnable(VkBool32 enable);
    void setDepthWriteEnable(VkBool32 enable);
    void setDepthCompareOp(VkCompareOp compare_op);

    void addDynamicState(VkDynamicState dynamic_state);

    VkPipeline create(const std::vector<SpirvData>& shaders) const;
};

#endif /* GPU_VULKAN_BUILDER_VULKANGRAPHICSPIPELINEBUILDER_H_ */
