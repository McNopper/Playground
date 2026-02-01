#include "MaterialShader.h"

#include <spirv-reflect/spirv_reflect.h>

#include "gpu/shader/shader.h"
#include "gpu/vulkan/factory/VulkanShaderModuleFactory.h"
#include "gpu/vulkan/factory/VulkanDescriptorSetLayoutFactory.h"
#include "gpu/vulkan/factory/VulkanPipelineLayoutFactory.h"
#include "gpu/vulkan/spirv/VulkanSpirvQuery.h"
#include "gpu/vulkan/spirv/vulkan_spirv.h"

#include "engine/renderer/backend/common/buffer/UniformBuffer.h"
#include "engine/renderer/backend/common/buffer/StorageBuffer.h"
#include "engine/renderer/backend/common/image/Texture.h"
#include "engine/renderer/backend/common/image/Sampler.h"

MaterialShader::MaterialShader(VkPhysicalDevice physical_device, VkDevice device, const std::string& shader_filename, const std::string& include_path) :
    m_physical_device{ physical_device }, m_device{ device }, m_shader_filename{ shader_filename }, m_include_path{ include_path }
{
}

MaterialShader::~MaterialShader()
{
    for (const auto& [stage, shader_module] : m_shader_modules)
    {
        if (shader_module != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(m_device, shader_module, nullptr);
        }
    }

    if (m_pipeline_layout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
    }
}

bool MaterialShader::build()
{
    // Build macros specific to this material
    std::map<std::string, std::string> macros{};
    // Derived classes can override this method to add their own macros

    // Compile Slang shader to SPIR-V
    // SpirvData includes both code and execution_model - keep both for pipeline creation
    m_spirv_shaders = buildSlang(m_shader_filename, macros, m_include_path);
    if (m_spirv_shaders.empty())
    {
        return false;
    }

    // Create Vulkan shader modules from SPIR-V, mapped by shader stage
    m_shader_modules.clear();

    for (const auto& spirv_data : m_spirv_shaders)
    {
        auto stage_opt = toVulkanShaderStage(spirv_data.execution_model);
        if (!stage_opt.has_value())
        {
            return false;
        }

        VulkanShaderModuleFactory shader_module_factory{ m_device, spirv_data.code };
        VkShaderModule shader_module = shader_module_factory.create();
        if (shader_module == VK_NULL_HANDLE)
        {
            return false;
        }

        m_shader_modules[*stage_opt] = shader_module;
    }

    // Use SPIR-V reflection to automatically generate pipeline layout
    VulkanSpirvQuery spirv_query{ m_spirv_shaders };

    // Gather descriptor set layout bindings from shader reflection
    std::vector<VkDescriptorSetLayoutBinding> bindings = spirv_query.gatherDescriptorSetLayoutBindings();

    // Gather push constant ranges from shader reflection
    std::vector<VkPushConstantRange> push_constant_ranges = spirv_query.gatherPushConstantRanges();

    // Create descriptor set layout from reflected bindings
    VkDescriptorSetLayout descriptor_set_layout = VK_NULL_HANDLE;
    if (!bindings.empty())
    {
        VulkanDescriptorSetLayoutFactory descriptor_set_layout_factory{ m_device, 0u };
        for (const auto& binding : bindings)
        {
            descriptor_set_layout_factory.addDescriptorSetLayoutBinding(binding);
        }
        descriptor_set_layout = descriptor_set_layout_factory.create();
        if (descriptor_set_layout == VK_NULL_HANDLE)
        {
            return false;
        }
    }

    // Create pipeline layout
    VulkanPipelineLayoutFactory pipeline_layout_factory{ m_device, 0u };
    if (descriptor_set_layout != VK_NULL_HANDLE)
    {
        pipeline_layout_factory.addDescriptorSetLayout(descriptor_set_layout);
    }
    for (const auto& push_constant_range : push_constant_ranges)
    {
        pipeline_layout_factory.addPushConstantRange(push_constant_range);
    }

    m_pipeline_layout = pipeline_layout_factory.create();

    // Clean up temporary descriptor set layout
    if (descriptor_set_layout != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorSetLayout(m_device, descriptor_set_layout, nullptr);
    }

    if (m_pipeline_layout == VK_NULL_HANDLE)
    {
        return false;
    }

    return true;
}

const std::vector<SpirvData>& MaterialShader::getSpirvShaders() const
{
    return m_spirv_shaders;
}

VkPipelineLayout MaterialShader::getPipelineLayout() const
{
    return m_pipeline_layout;
}

bool MaterialShader::setUniformBuffer(const std::string& name, const UniformBuffer* buffer)
{
    if (!buffer || !buffer->isValid())
    {
        return false;
    }

    // Use SPIR-V reflection to find the descriptor by name
    for (const auto& spirv_data : m_spirv_shaders)
    {
        SpvReflectShaderModule shader_module{};
        SpvReflectResult result = spvReflectCreateShaderModule(
            spirv_data.code.size() * sizeof(uint32_t), 
            spirv_data.code.data(), 
            &shader_module
        );
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            continue;
        }

        uint32_t count{ 0u };
        result = spvReflectEnumerateDescriptorSets(&shader_module, &count, nullptr);
        if (result == SPV_REFLECT_RESULT_SUCCESS && count > 0)
        {
            std::vector<SpvReflectDescriptorSet*> descriptor_sets(count);
            result = spvReflectEnumerateDescriptorSets(&shader_module, &count, descriptor_sets.data());

            if (result == SPV_REFLECT_RESULT_SUCCESS)
            {
                for (uint32_t i = 0u; i < count; i++)
                {
                    for (uint32_t j = 0u; j < descriptor_sets[i]->binding_count; j++)
                    {
                        const SpvReflectDescriptorBinding* binding = descriptor_sets[i]->bindings[j];
                        
                        // Check if this is a uniform buffer and name matches
                        if (binding->name && name == binding->name)
                        {
                            VkDescriptorType desc_type = (VkDescriptorType)binding->descriptor_type;
                            if (desc_type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
                            {
                                uint32_t set = descriptor_sets[i]->set;
                                uint32_t binding_index = binding->binding;
                                m_uniform_buffers[{set, binding_index}] = buffer;

                                spvReflectDestroyShaderModule(&shader_module);
                                return true;
                            }
                        }
                    }
                }
            }
        }

        spvReflectDestroyShaderModule(&shader_module);
    }

    return false;
}

bool MaterialShader::setStorageBuffer(const std::string& name, const StorageBuffer* buffer)
{
    if (!buffer || !buffer->isValid())
    {
        return false;
    }

    // Use SPIR-V reflection to find the descriptor by name
    for (const auto& spirv_data : m_spirv_shaders)
    {
        SpvReflectShaderModule shader_module{};
        SpvReflectResult result = spvReflectCreateShaderModule(
            spirv_data.code.size() * sizeof(uint32_t), 
            spirv_data.code.data(), 
            &shader_module
        );
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            continue;
        }

        uint32_t count{ 0u };
        result = spvReflectEnumerateDescriptorSets(&shader_module, &count, nullptr);
        if (result == SPV_REFLECT_RESULT_SUCCESS && count > 0)
        {
            std::vector<SpvReflectDescriptorSet*> descriptor_sets(count);
            result = spvReflectEnumerateDescriptorSets(&shader_module, &count, descriptor_sets.data());

            if (result == SPV_REFLECT_RESULT_SUCCESS)
            {
                for (uint32_t i = 0u; i < count; i++)
                {
                    for (uint32_t j = 0u; j < descriptor_sets[i]->binding_count; j++)
                    {
                        const SpvReflectDescriptorBinding* binding = descriptor_sets[i]->bindings[j];
                        
                        // Check if this is a storage buffer and name matches
                        if (binding->name && name == binding->name)
                        {
                            VkDescriptorType desc_type = (VkDescriptorType)binding->descriptor_type;
                            if (desc_type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
                            {
                                uint32_t set = descriptor_sets[i]->set;
                                uint32_t binding_index = binding->binding;
                                m_storage_buffers[{set, binding_index}] = buffer;

                                spvReflectDestroyShaderModule(&shader_module);
                                return true;
                            }
                        }
                    }
                }
            }
        }

        spvReflectDestroyShaderModule(&shader_module);
    }

    return false;
}

bool MaterialShader::setTexture(const std::string& name, const Texture* texture)
{
    if (!texture || !texture->isValid())
    {
        return false;
    }

    // Use SPIR-V reflection to find the descriptor by name
    for (const auto& spirv_data : m_spirv_shaders)
    {
        SpvReflectShaderModule shader_module{};
        SpvReflectResult result = spvReflectCreateShaderModule(
            spirv_data.code.size() * sizeof(uint32_t), 
            spirv_data.code.data(), 
            &shader_module
        );
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            continue;
        }

        uint32_t count{ 0u };
        result = spvReflectEnumerateDescriptorSets(&shader_module, &count, nullptr);
        if (result == SPV_REFLECT_RESULT_SUCCESS && count > 0)
        {
            std::vector<SpvReflectDescriptorSet*> descriptor_sets(count);
            result = spvReflectEnumerateDescriptorSets(&shader_module, &count, descriptor_sets.data());

            if (result == SPV_REFLECT_RESULT_SUCCESS)
            {
                for (uint32_t i = 0u; i < count; i++)
                {
                    for (uint32_t j = 0u; j < descriptor_sets[i]->binding_count; j++)
                    {
                        const SpvReflectDescriptorBinding* binding = descriptor_sets[i]->bindings[j];
                        
                        // Check if this is a texture/image and name matches
                        if (binding->name && name == binding->name)
                        {
                            VkDescriptorType desc_type = (VkDescriptorType)binding->descriptor_type;
                            if (desc_type == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE || 
                                desc_type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
                                desc_type == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
                            {
                                uint32_t set = descriptor_sets[i]->set;
                                uint32_t binding_index = binding->binding;
                                m_textures[{set, binding_index}] = texture;

                                spvReflectDestroyShaderModule(&shader_module);
                                return true;
                            }
                        }
                    }
                }
            }
        }

        spvReflectDestroyShaderModule(&shader_module);
    }

    return false;
}

bool MaterialShader::setSampler(const std::string& name, const Sampler* sampler)
{
    if (!sampler || !sampler->isValid())
    {
        return false;
    }

    // Use SPIR-V reflection to find the descriptor by name
    for (const auto& spirv_data : m_spirv_shaders)
    {
        SpvReflectShaderModule shader_module{};
        SpvReflectResult result = spvReflectCreateShaderModule(
            spirv_data.code.size() * sizeof(uint32_t), 
            spirv_data.code.data(), 
            &shader_module
        );
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            continue;
        }

        uint32_t count{ 0u };
        result = spvReflectEnumerateDescriptorSets(&shader_module, &count, nullptr);
        if (result == SPV_REFLECT_RESULT_SUCCESS && count > 0)
        {
            std::vector<SpvReflectDescriptorSet*> descriptor_sets(count);
            result = spvReflectEnumerateDescriptorSets(&shader_module, &count, descriptor_sets.data());

            if (result == SPV_REFLECT_RESULT_SUCCESS)
            {
                for (uint32_t i = 0u; i < count; i++)
                {
                    for (uint32_t j = 0u; j < descriptor_sets[i]->binding_count; j++)
                    {
                        const SpvReflectDescriptorBinding* binding = descriptor_sets[i]->bindings[j];
                        
                        // Check if this is a sampler and name matches
                        if (binding->name && name == binding->name)
                        {
                            VkDescriptorType desc_type = (VkDescriptorType)binding->descriptor_type;
                            if (desc_type == VK_DESCRIPTOR_TYPE_SAMPLER ||
                                desc_type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
                            {
                                uint32_t set = descriptor_sets[i]->set;
                                uint32_t binding_index = binding->binding;
                                m_samplers[{set, binding_index}] = sampler;

                                spvReflectDestroyShaderModule(&shader_module);
                                return true;
                            }
                        }
                    }
                }
            }
        }

        spvReflectDestroyShaderModule(&shader_module);
    }

    return false;
}

void MaterialShader::bind(VkCommandBuffer command_buffer) const
{
    // TODO: Implement descriptor binding
    // This requires creating descriptor sets/buffers from the attached resources
    // For now, this is a placeholder for future implementation
    // Users can still manually bind using the current approach
}

