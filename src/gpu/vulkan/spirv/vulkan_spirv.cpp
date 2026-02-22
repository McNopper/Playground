#include "vulkan_spirv.h"

std::optional<VkShaderStageFlagBits> toVulkanShaderStage(SpvExecutionModel execution_model)
{
    // Current execution models, which can be extended.
    switch (execution_model)
    {
        case SpvExecutionModelVertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case SpvExecutionModelTessellationControl:
            return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case SpvExecutionModelTessellationEvaluation:
            return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        case SpvExecutionModelGeometry:
            return VK_SHADER_STAGE_GEOMETRY_BIT;
        case SpvExecutionModelFragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case SpvExecutionModelGLCompute:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        
        default:
            break;
    }

    return {};
}

std::optional<VkShaderStageFlagBits> toVulkanShaderStage(SpvReflectShaderStageFlagBits execution_model)
{
    // Current execution models, which can be extended.
    switch (execution_model)
    {
    case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
        return VK_SHADER_STAGE_VERTEX_BIT;
    case SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
        return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    case SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
        return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    case SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT:
        return VK_SHADER_STAGE_GEOMETRY_BIT;
    case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:
        return VK_SHADER_STAGE_FRAGMENT_BIT;
    case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT:
        return VK_SHADER_STAGE_COMPUTE_BIT;

    default:
        break;
    }

    return {};
}
