#ifndef GPU_VULKAN_SPIRV_VULKANSPIRV_H_
#define GPU_VULKAN_SPIRV_VULKANSPIRV_H_

#include <optional>

#include <spirv-reflect/spirv_reflect.h>

#include <volk.h>

std::optional<VkShaderStageFlagBits> toVulkanShaderStage(SpvExecutionModel execution_model);

std::optional<VkShaderStageFlagBits> toVulkanShaderStage(SpvReflectShaderStageFlagBits execution_model);

#endif /* GPU_VULKAN_SPIRV_VULKANSPIRV_H_ */