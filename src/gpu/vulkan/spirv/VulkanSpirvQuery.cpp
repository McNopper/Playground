#include "VulkanSpirvQuery.h"

#include <algorithm>

#include "core/utility/strings.h"
#include "gpu/vulkan/utility/vulkan_helper.h"
#include "vulkan_spirv.h"

void VulkanSpirvQuery::destroyShaderModules(std::size_t max)
{
    max = std::min(m_shader_modules.size(), max);

    for (std::size_t i = 0u; i < max; i++)
    {
        spvReflectDestroyShaderModule(&m_shader_modules[i]);
    }
    m_shader_modules.clear();
}

VulkanSpirvQuery::VulkanSpirvQuery(const std::vector<SpirvData>& shaders)
{
    m_shader_modules.resize(shaders.size());

    for (std::size_t i = 0u; i < shaders.size(); i++)
    {
        SpvReflectResult result = spvReflectCreateShaderModule(shaders[i].code.size() * sizeof(uint32_t), shaders[i].code.data(), &m_shader_modules[i]);
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            destroyShaderModules(i);
            return;
        }
    }
}

VulkanSpirvQuery::~VulkanSpirvQuery()
{
    destroyShaderModules(m_shader_modules.size());
}

std::vector<VkDescriptorSetLayoutBinding> VulkanSpirvQuery::gatherDescriptorSetLayoutBindings() const
{
    std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings{};

    for (uint32_t i = 0u; i < (uint32_t)m_shader_modules.size(); i++)
    {
        auto stage = toVulkanShaderStage(m_shader_modules[i].shader_stage);
        if (!stage.has_value())
        {
            return {};
        }

        uint32_t count{ 0u };
        SpvReflectResult result = spvReflectEnumerateDescriptorSets(&m_shader_modules[i], &count, nullptr);
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            return {};
        }

        std::vector<SpvReflectDescriptorSet*> descriptor_sets(count);
        result = spvReflectEnumerateDescriptorSets(&m_shader_modules[i], &count, descriptor_sets.data());
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            return {};
        }

        for (uint32_t j = 0u; j < count; j++)
        {
            for (uint32_t k = 0u; k < descriptor_sets[j]->binding_count; k++)
            {
                VkDescriptorSetLayoutBinding descriptor_set_layout_binding{};
                descriptor_set_layout_binding.binding = descriptor_sets[j]->bindings[k]->binding;
                descriptor_set_layout_binding.descriptorType = (VkDescriptorType)descriptor_sets[j]->bindings[k]->descriptor_type;
                descriptor_set_layout_binding.descriptorCount = 1u;
                for (uint32_t l = 0u; l < descriptor_sets[j]->bindings[k]->array.dims_count; l++)
                {
                    descriptor_set_layout_binding.descriptorCount *= descriptor_sets[j]->bindings[k]->array.dims[l];
                }
                descriptor_set_layout_binding.stageFlags = *stage;
                descriptor_set_layout_binding.pImmutableSamplers = nullptr;

                // Check, if already added.

                bool merged{ false };
                for (auto& compare : descriptor_set_layout_bindings)
                {
                    if (compare.binding == descriptor_set_layout_binding.binding)
                    {
                        compare.stageFlags |= descriptor_set_layout_binding.stageFlags;
                        merged = true;
                        break;
                    }
                }

                if (!merged)
                {
                    descriptor_set_layout_bindings.push_back(descriptor_set_layout_binding);
                }
            }
        }
    }

    return descriptor_set_layout_bindings;
}

std::vector<std::string> VulkanSpirvQuery::gatherDescriptorSetBlockNames() const
{
    std::vector<std::string> names{};

    for (uint32_t i = 0u; i < (uint32_t)m_shader_modules.size(); i++)
    {
        auto stage = toVulkanShaderStage(m_shader_modules[i].shader_stage);
        if (!stage.has_value())
        {
            return {};
        }

        uint32_t count{ 0u };
        SpvReflectResult result = spvReflectEnumerateDescriptorSets(&m_shader_modules[i], &count, nullptr);
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            return {};
        }

        std::vector<SpvReflectDescriptorSet*> descriptor_sets(count);
        result = spvReflectEnumerateDescriptorSets(&m_shader_modules[i], &count, descriptor_sets.data());
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            return {};
        }

        for (uint32_t j = 0u; j < count; j++)
        {
            for (uint32_t k = 0u; k < descriptor_sets[j]->binding_count; k++)
            {
                if (descriptor_sets[j]->bindings[k]->block.name == nullptr)
                {
                    continue;
                }

                std::string name = std::string(descriptor_sets[j]->bindings[k]->block.name);

                if (!name.empty() && std::find(names.begin(), names.end(), name) == names.end())
                {
                    names.push_back(name);
                }
            }
        }
    }

    return names;
}

std::vector<std::string> VulkanSpirvQuery::gatherDescriptorSetBlockMemberNames() const
{
    std::vector<std::string> names{};

    for (uint32_t i = 0u; i < (uint32_t)m_shader_modules.size(); i++)
    {
        auto stage = toVulkanShaderStage(m_shader_modules[i].shader_stage);
        if (!stage.has_value())
        {
            return {};
        }

        uint32_t count{ 0u };
        SpvReflectResult result = spvReflectEnumerateDescriptorSets(&m_shader_modules[i], &count, nullptr);
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            return {};
        }

        std::vector<SpvReflectDescriptorSet*> descriptor_sets(count);
        result = spvReflectEnumerateDescriptorSets(&m_shader_modules[i], &count, descriptor_sets.data());
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            return {};
        }

        for (uint32_t j = 0u; j < count; j++)
        {
            for (uint32_t k = 0u; k < descriptor_sets[j]->binding_count; k++)
            {
                if (descriptor_sets[j]->bindings[k]->block.name == nullptr)
                {
                    continue;
                }

                for (uint32_t l = 0u; l < descriptor_sets[j]->bindings[k]->block.member_count; l++)
                {
                    if (descriptor_sets[j]->bindings[k]->block.members[l].name == nullptr)
                    {
                        continue;
                    }

                    std::string name = std::string(descriptor_sets[j]->bindings[k]->block.name) + "." + std::string(descriptor_sets[j]->bindings[k]->block.members[l].name);

                    if (std::find(names.begin(), names.end(), name) == names.end())
                    {
                        names.push_back(name);
                    }
                }
            }
        }
    }

    return names;
}

std::optional<DescriptorBufferInfo> VulkanSpirvQuery::gatherDescriptorSetBufferInfo(const std::string& name) const
{
    for (uint32_t i = 0u; i < (uint32_t)m_shader_modules.size(); i++)
    {
        auto stage = toVulkanShaderStage(m_shader_modules[i].shader_stage);
        if (!stage.has_value())
        {
            return {};
        }

        uint32_t count{ 0u };
        SpvReflectResult result = spvReflectEnumerateDescriptorSets(&m_shader_modules[i], &count, nullptr);
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            return {};
        }

        std::vector<SpvReflectDescriptorSet*> descriptor_sets(count);
        result = spvReflectEnumerateDescriptorSets(&m_shader_modules[i], &count, descriptor_sets.data());
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            return {};
        }

        for (uint32_t j = 0u; j < count; j++)
        {
            for (uint32_t k = 0u; k < descriptor_sets[j]->binding_count; k++)
            {
                if (descriptor_sets[j]->bindings[k]->block.name == nullptr)
                {
                    continue;
                }

                std::string block_name = std::string(descriptor_sets[j]->bindings[k]->block.name);

                if (block_name == name)
                {
                    return DescriptorBufferInfo{
                        (VkDeviceSize)descriptor_sets[j]->bindings[k]->block.offset,
                        (VkDeviceSize)descriptor_sets[j]->bindings[k]->block.size
                    };
                }

                for (uint32_t l = 0u; l < descriptor_sets[j]->bindings[k]->block.member_count; l++)
                {
                    if (descriptor_sets[j]->bindings[k]->block.members[l].name == nullptr)
                    {
                        continue;
                    }

                    std::string current_name = block_name + "." + std::string(descriptor_sets[j]->bindings[k]->block.members[l].name);

                    if (current_name == name)
                    {
                        return DescriptorBufferInfo{
                            (VkDeviceSize)descriptor_sets[j]->bindings[k]->block.members[l].offset,
                            (VkDeviceSize)descriptor_sets[j]->bindings[k]->block.members[l].size
                        };
                    }
                }
            }
        }
    }

    return {};
}

std::vector<VkPushConstantRange> VulkanSpirvQuery::gatherPushConstantRanges() const
{
    std::vector<VkPushConstantRange> push_constant_ranges{};

    for (uint32_t i = 0u; i < (uint32_t)m_shader_modules.size(); i++)
    {
        auto stage = toVulkanShaderStage(m_shader_modules[i].shader_stage);
        if (!stage.has_value())
        {
            return {};
        }

        uint32_t count{ 0u };
        SpvReflectResult result = spvReflectEnumeratePushConstantBlocks(&m_shader_modules[i], &count, nullptr);
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            return {};
        }

        std::vector<SpvReflectBlockVariable*> block_variables(count);
        result = spvReflectEnumeratePushConstantBlocks(&m_shader_modules[i], &count, block_variables.data());
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            return {};
        }

        // Merge stage flags, as push constants can also used in another shader stages.

        for (uint32_t j = 0u; j < count; j++)
        {
            VkPushConstantRange push_constant_range{};
            push_constant_range.stageFlags = *stage;
            push_constant_range.offset = block_variables[j]->absolute_offset;
            push_constant_range.size = block_variables[j]->padded_size;

            bool merged{ false };
            for (auto& compare : push_constant_ranges)
            {
                if (compare.offset == push_constant_range.offset && compare.size == push_constant_range.size)
                {
                    compare.stageFlags |= push_constant_range.stageFlags;
                    merged = true;
                    break;
                }
            }

            if (!merged)
            {
                push_constant_ranges.push_back(push_constant_range);
            }
        }
    }

    return push_constant_ranges;
}

std::optional<VkVertexInputBindingDescription> VulkanSpirvQuery::gatherVertexInputBindingDescription(uint32_t binding) const
{
    for (uint32_t i = 0u; i < (uint32_t)m_shader_modules.size(); i++)
    {
        if (m_shader_modules[i].shader_stage != SPV_REFLECT_SHADER_STAGE_VERTEX_BIT)
        {
            continue;
        }

        uint32_t count{ 0u };
        SpvReflectResult result = spvReflectEnumerateInputVariables(&m_shader_modules[i], &count, nullptr);
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            return {};
        }

        std::vector<SpvReflectInterfaceVariable*> input_variables(count);
        result = spvReflectEnumerateInputVariables(&m_shader_modules[i], &count, input_variables.data());
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            return {};
        }

        // Sort by location.
        std::sort(std::begin(input_variables), std::end(input_variables),
                  [](const SpvReflectInterfaceVariable* a, const SpvReflectInterfaceVariable* b) {
                      return a->location < b->location;
                  });

        VkVertexInputBindingDescription vertex_input_binding_description{};

        vertex_input_binding_description.binding = binding;
        vertex_input_binding_description.stride = 0u;
        vertex_input_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        for (uint32_t j = 0u; j < (uint32_t)input_variables.size(); j++)
        {
            if (input_variables[j]->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN)
            {
                continue;
            }

            auto format_size = getFormatSize((VkFormat)input_variables[j]->format);
            if (format_size == 0u)
            {
                return {};
            }

            vertex_input_binding_description.stride += format_size;
        }

        return vertex_input_binding_description;
    }

    return {};
}

std::vector<VkVertexInputAttributeDescription> VulkanSpirvQuery::gatherVertexInputAttributeDescriptions(uint32_t binding) const
{
    std::vector<VkVertexInputAttributeDescription> vertex_input_attribute_descriptions{};

    for (uint32_t i = 0u; i < (uint32_t)m_shader_modules.size(); i++)
    {
        if (m_shader_modules[i].shader_stage != SPV_REFLECT_SHADER_STAGE_VERTEX_BIT)
        {
            continue;
        }

        uint32_t count{ 0u };
        SpvReflectResult result = spvReflectEnumerateInputVariables(&m_shader_modules[i], &count, nullptr);
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            return {};
        }

        std::vector<SpvReflectInterfaceVariable*> input_variables(count);
        result = spvReflectEnumerateInputVariables(&m_shader_modules[i], &count, input_variables.data());
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            return {};
        }

        // Sort by location.
        std::sort(std::begin(input_variables), std::end(input_variables),
                  [](const SpvReflectInterfaceVariable* a, const SpvReflectInterfaceVariable* b) {
                      return a->location < b->location;
                  });

        vertex_input_attribute_descriptions.resize(input_variables.size());

        uint32_t stride{ 0u };
        for (uint32_t j = 0u; j < (uint32_t)input_variables.size(); j++)
        {
            if (input_variables[j]->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN)
            {
                continue;
            }

            auto format = (VkFormat)input_variables[j]->format;
            if (format == VK_FORMAT_UNDEFINED)
            {
                return {};
            }

            auto format_size = getFormatSize(format);
            if (format_size == 0u)
            {
                return {};
            }

            vertex_input_attribute_descriptions[j].location = input_variables[j]->location;
            vertex_input_attribute_descriptions[j].binding = binding;
            vertex_input_attribute_descriptions[j].format = format;
            vertex_input_attribute_descriptions[j].offset = stride;

            stride += format_size;
        }

        return vertex_input_attribute_descriptions;
    }

    return {};
}

std::vector<std::string> VulkanSpirvQuery::gatherVertexInputNames() const
{
    std::vector<std::string> names{};

    for (uint32_t i = 0u; i < (uint32_t)m_shader_modules.size(); i++)
    {
        if (m_shader_modules[i].shader_stage != SPV_REFLECT_SHADER_STAGE_VERTEX_BIT)
        {
            continue;
        }

        uint32_t count{ 0u };
        SpvReflectResult result = spvReflectEnumerateInputVariables(&m_shader_modules[i], &count, nullptr);
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            return {};
        }

        std::vector<SpvReflectInterfaceVariable*> input_variables(count);
        result = spvReflectEnumerateInputVariables(&m_shader_modules[i], &count, input_variables.data());
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            return {};
        }

        // Sort by location.
        std::sort(std::begin(input_variables), std::end(input_variables),
                  [](const SpvReflectInterfaceVariable* a, const SpvReflectInterfaceVariable* b) {
                      return a->location < b->location;
                  });

        for (uint32_t j = 0u; j < (uint32_t)input_variables.size(); j++)
        {
            if (input_variables[j]->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN)
            {
                continue;
            }

            names.push_back(afterString(input_variables[j]->name, "."));
        }

        return names;
    }

    return {};
}
