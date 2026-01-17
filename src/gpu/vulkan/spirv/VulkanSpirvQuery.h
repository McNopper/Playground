#ifndef GPU_VULKAN_SPIRV_VULKANSPIRVQUERY_H_
#define GPU_VULKAN_SPIRV_VULKANSPIRVQUERY_H_

#include <optional>
#include <string>
#include <vector>

#include <spirv-reflect/spirv_reflect.h>

#include <volk.h>

#include "gpu/shader/spirv/spirv_data.h"

struct DescriptorBufferInfo {
	VkDeviceSize offset{ 0u };
	VkDeviceSize range{ 0u };
};

class VulkanSpirvQuery {

private:

	std::vector<SpvReflectShaderModule> m_shader_modules{};

	void destroyShaderModules(std::size_t max);

public:

	VulkanSpirvQuery() = delete;

	explicit VulkanSpirvQuery(const std::vector<SpirvData>& shaders);

	~VulkanSpirvQuery();

	// Uniform

	std::vector<VkDescriptorSetLayoutBinding> gatherDescriptorSetLayoutBindings() const;

	std::vector<std::string> gatherDescriptorSetBlockNames() const;

	std::vector<std::string> gatherDescriptorSetBlockMemberNames() const;

	std::optional<DescriptorBufferInfo> gatherDescriptorSetBufferInfo(const std::string& name) const;

	std::vector<VkPushConstantRange> gatherPushConstantRanges() const;

	// Vertex

	std::optional<VkVertexInputBindingDescription> gatherVertexInputBindingDescription(uint32_t binding) const;

	std::vector<VkVertexInputAttributeDescription> gatherVertexInputAttributeDescriptions(uint32_t binding) const;

	std::vector<std::string> gatherVertexInputNames() const;

};

#endif /* GPU_VULKAN_SPIRV_VULKANSPIRVQUERY_H_ */