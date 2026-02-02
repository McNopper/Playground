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
#include "engine/renderer/backend/common/buffer/DescriptorBufferSet.h"
#include "engine/renderer/backend/common/image/Texture.h"
#include "engine/renderer/backend/common/image/Texture2D.h"
#include "engine/renderer/backend/common/image/Sampler.h"
#include "engine/renderer/material/Texture2DSampler.h"

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
    // We need to handle multiple descriptor sets (set 0, set 1, etc.)
    
    // Build descriptor info list first to know which sets exist
    if (!buildDescriptorInfos())
    {
        return false;
    }

    // Group descriptors by set
    std::map<uint32_t, std::vector<DescriptorInfo>> descriptors_by_set{};
    for (const auto& info : m_descriptor_infos)
    {
        descriptors_by_set[info.set].push_back(info);
    }

    // Create descriptor set layout for each set
    std::vector<VkDescriptorSetLayout> descriptor_set_layouts{};
    VkDescriptorSetLayoutCreateFlags layout_flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_DESCRIPTOR_BUFFER_BIT_EXT;
    
    for (const auto& [set_index, descriptors] : descriptors_by_set)
    {
        VulkanDescriptorSetLayoutFactory descriptor_set_layout_factory{ m_device, layout_flags };
        
        for (const auto& info : descriptors)
        {
            // Find the stage flags from SPIR-V reflection
            VkShaderStageFlags stage_flags = 0;
            for (const auto& spirv_data : m_spirv_shaders)
            {
                auto stage_opt = toVulkanShaderStage(spirv_data.execution_model);
                if (stage_opt.has_value())
                {
                    // Check if this descriptor exists in this shader stage
                    SpvReflectShaderModule shader_module{};
                    SpvReflectResult result = spvReflectCreateShaderModule(
                        spirv_data.code.size() * sizeof(uint32_t), 
                        spirv_data.code.data(), 
                        &shader_module
                    );
                    if (result == SPV_REFLECT_RESULT_SUCCESS)
                    {
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
                                    if (descriptor_sets[i]->set == info.set)
                                    {
                                        for (uint32_t j = 0u; j < descriptor_sets[i]->binding_count; j++)
                                        {
                                            const SpvReflectDescriptorBinding* binding = descriptor_sets[i]->bindings[j];
                                            if (binding->binding == info.binding)
                                            {
                                                stage_flags |= *stage_opt;
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        spvReflectDestroyShaderModule(&shader_module);
                    }
                }
            }

            VkDescriptorSetLayoutBinding binding{};
            binding.binding = info.binding;
            binding.descriptorType = info.type;
            binding.descriptorCount = 1;
            binding.stageFlags = stage_flags;
            binding.pImmutableSamplers = nullptr;
            descriptor_set_layout_factory.addDescriptorSetLayoutBinding(binding);
        }
        
        VkDescriptorSetLayout descriptor_set_layout = descriptor_set_layout_factory.create();
        if (descriptor_set_layout == VK_NULL_HANDLE)
        {
            // Clean up previously created layouts
            for (auto* layout : descriptor_set_layouts)
            {
                vkDestroyDescriptorSetLayout(m_device, layout, nullptr);
            }
            return false;
        }
        descriptor_set_layouts.push_back(descriptor_set_layout);
    }

    // Gather push constant ranges from shader reflection
    VulkanSpirvQuery spirv_query{ m_spirv_shaders };
    std::vector<VkPushConstantRange> push_constant_ranges = spirv_query.gatherPushConstantRanges();

    // Create pipeline layout with all descriptor set layouts
    VulkanPipelineLayoutFactory pipeline_layout_factory{ m_device, 0u };
    for (auto* descriptor_set_layout : descriptor_set_layouts)
    {
        pipeline_layout_factory.addDescriptorSetLayout(descriptor_set_layout);
    }
    for (const auto& push_constant_range : push_constant_ranges)
    {
        pipeline_layout_factory.addPushConstantRange(push_constant_range);
    }

    m_pipeline_layout = pipeline_layout_factory.create();

    // Clean up temporary descriptor set layouts
    for (auto* descriptor_set_layout : descriptor_set_layouts)
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

bool MaterialShader::setTexture2DSampler(const std::string& texture_name, const std::string& sampler_name, const Texture2DSampler* texture_sampler)
{
    if (!texture_sampler)
    {
        return false;
    }

    const Texture2D* texture = texture_sampler->getTexture();
    const Sampler* sampler = texture_sampler->getSampler();

    if (!texture || !sampler)
    {
        return false;
    }

    bool texture_set = setTexture(texture_name, texture);
    bool sampler_set = setSampler(sampler_name, sampler);

    return texture_set && sampler_set;
}

void MaterialShader::bind(VkCommandBuffer command_buffer) const
{
    if (!m_descriptors_built || m_descriptor_sets.empty())
    {
        return;
    }

    // Prepare descriptor buffer bindings
    std::vector<VkDescriptorBufferBindingInfoEXT> descriptor_buffer_bindings{};
    descriptor_buffer_bindings.reserve(m_descriptor_sets.size());

    for (const auto& [set_index, descriptor_set] : m_descriptor_sets)
    {
        VkDescriptorBufferBindingInfoEXT binding{};
        binding.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT;
        binding.address = descriptor_set->getDeviceAddress();
        binding.usage = descriptor_set->getUsageFlags();
        descriptor_buffer_bindings.push_back(binding);
    }

    // Bind descriptor buffers
    vkCmdBindDescriptorBuffersEXT(command_buffer, static_cast<uint32_t>(descriptor_buffer_bindings.size()), descriptor_buffer_bindings.data());

    // Set descriptor buffer offsets for each set
    uint32_t buffer_index = 0;
    for (const auto& [set_index, descriptor_set] : m_descriptor_sets)
    {
        VkDeviceSize offset = 0;
        vkCmdSetDescriptorBufferOffsetsEXT(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_layout, set_index, 1, &buffer_index, &offset);
        buffer_index++;
    }
}

bool MaterialShader::buildDescriptorInfos()
{
    m_descriptor_infos.clear();

    // Use SPIR-V reflection to gather all descriptor info
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
            // Failed to reflect this shader - this is a critical error
            return false;
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
                        
                        DescriptorInfo info{};
                        info.set = descriptor_sets[i]->set;
                        info.binding = binding->binding;
                        info.type = static_cast<VkDescriptorType>(binding->descriptor_type);
                        
                        m_descriptor_infos.push_back(info);
                    }
                }
            }
        }

        spvReflectDestroyShaderModule(&shader_module);
    }

    // Descriptor info can be empty (shader has no descriptors), which is valid
    return true;
}

bool MaterialShader::buildDescriptors()
{
    if (m_descriptors_built)
    {
        return true;
    }

    // Group descriptors by set
    std::map<uint32_t, std::vector<DescriptorInfo>> descriptors_by_set{};
    for (const auto& info : m_descriptor_infos)
    {
        descriptors_by_set[info.set].push_back(info);
    }

    // Create descriptor buffer for each set
    for (const auto& [set_index, descriptors] : descriptors_by_set)
    {
        auto descriptor_set = std::make_unique<DescriptorBufferSet>(m_physical_device, m_device);

        // Add bindings
        for (const auto& info : descriptors)
        {
            descriptor_set->addBinding(info.binding, info.type);
        }

        // Create the descriptor buffer
        if (!descriptor_set->create())
        {
            return false;
        }

        // Write descriptors from attached resources
        for (const auto& info : descriptors)
        {
            std::pair<uint32_t, uint32_t> key{ info.set, info.binding };

            // Check each resource type
            if (info.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            {
                auto it = m_uniform_buffers.find(key);
                if (it != m_uniform_buffers.end() && it->second)
                {
                    VkDescriptorBufferInfo buffer_info{};
                    buffer_info.buffer = it->second->getBuffer();
                    buffer_info.offset = 0u;
                    buffer_info.range = it->second->getDeviceSize();
                    descriptor_set->writeBufferDescriptor(info.binding, buffer_info);
                }
            }
            else if (info.type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
            {
                auto it = m_storage_buffers.find(key);
                if (it != m_storage_buffers.end() && it->second)
                {
                    VkDescriptorBufferInfo buffer_info{};
                    buffer_info.buffer = it->second->getBuffer();
                    buffer_info.offset = 0u;
                    buffer_info.range = it->second->getDeviceSize();
                    descriptor_set->writeBufferDescriptor(info.binding, buffer_info);
                }
            }
            else if (info.type == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE || info.type == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
            {
                auto it = m_textures.find(key);
                if (it != m_textures.end() && it->second)
                {
                    VkDescriptorImageInfo image_info{};
                    image_info.sampler = VK_NULL_HANDLE;
                    image_info.imageView = it->second->getImageView();
                    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    descriptor_set->writeImageDescriptor(info.binding, image_info);
                }
            }
            else if (info.type == VK_DESCRIPTOR_TYPE_SAMPLER)
            {
                auto it = m_samplers.find(key);
                if (it != m_samplers.end() && it->second)
                {
                    VkDescriptorImageInfo sampler_info{};
                    sampler_info.sampler = it->second->getSampler();
                    sampler_info.imageView = VK_NULL_HANDLE;
                    sampler_info.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                    descriptor_set->writeImageDescriptor(info.binding, sampler_info);
                }
            }
        }

        m_descriptor_sets[set_index] = std::move(descriptor_set);
    }

    m_descriptors_built = true;
    return true;
}

