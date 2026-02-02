#ifndef ENGINE_RENDERER_MATERIAL_MATERIALSHADER_H_
#define ENGINE_RENDERER_MATERIAL_MATERIALSHADER_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <volk.h>

#include "gpu/shader/spirv/spirv_data.h"

// Forward declarations
class UniformBuffer;
class StorageBuffer;
class Texture;
class Sampler;
class Texture2DSampler;
class DescriptorBufferSet;

// Base class for material shaders
// Takes a Slang shader file, compiles it to SPIR-V, and reflects shader resources
// Stores both SpirvData (with execution_model) and VkShaderModule for pipeline creation
// Manages textures, samplers, and uniform buffers attached to the material
// Automatically creates and manages descriptor buffers for bound resources
// Inspired by OpenUSD's UsdShadeShader and MaterialX node conventions
class MaterialShader
{

private:

    VkPhysicalDevice m_physical_device{ VK_NULL_HANDLE };
    VkDevice m_device{ VK_NULL_HANDLE };

    std::string m_shader_filename{};
    std::string m_include_path{};

    std::vector<SpirvData> m_spirv_shaders{};
    std::map<VkShaderStageFlagBits, VkShaderModule> m_shader_modules{};

    VkPipelineLayout m_pipeline_layout{ VK_NULL_HANDLE };

    // Resource storage - indexed by (set, binding) pair
    std::map<std::pair<uint32_t, uint32_t>, const UniformBuffer*> m_uniform_buffers{};
    std::map<std::pair<uint32_t, uint32_t>, const StorageBuffer*> m_storage_buffers{};
    std::map<std::pair<uint32_t, uint32_t>, const Texture*> m_textures{};
    std::map<std::pair<uint32_t, uint32_t>, const Sampler*> m_samplers{};

    // Descriptor buffers - one per descriptor set
    std::map<uint32_t, std::unique_ptr<DescriptorBufferSet>> m_descriptor_sets{};

    // Descriptor set layout info from reflection
    struct DescriptorInfo
    {
        uint32_t set{ 0u };
        uint32_t binding{ 0u };
        VkDescriptorType type{ VK_DESCRIPTOR_TYPE_MAX_ENUM };
    };
    std::vector<DescriptorInfo> m_descriptor_infos{};

    bool m_descriptors_built{ false };

    bool buildDescriptorInfos();

public:

    MaterialShader() = delete;

    MaterialShader(VkPhysicalDevice physical_device, VkDevice device, const std::string& shader_filename, const std::string& include_path);

    virtual ~MaterialShader();

    MaterialShader(const MaterialShader& other) = delete;
    MaterialShader& operator=(const MaterialShader& other) = delete;

    bool build();

    // Resource attachment by shader variable name
    // Returns true if resource exists in shader, false otherwise
    // Note: MaterialShader does not own these resources - caller must ensure lifetime
    bool setUniformBuffer(const std::string& name, const UniformBuffer* buffer);
    bool setStorageBuffer(const std::string& name, const StorageBuffer* buffer);
    bool setTexture(const std::string& name, const Texture* texture);
    bool setSampler(const std::string& name, const Sampler* sampler);

    // Convenience method for combined texture+sampler
    bool setTexture2DSampler(const std::string& texture_name, const std::string& sampler_name, const Texture2DSampler* texture_sampler);

    // Build descriptor buffers from attached resources
    // Call this after setting all resources and before rendering
    bool buildDescriptors();

    // Bind descriptor buffers for rendering
    void bind(VkCommandBuffer command_buffer) const;

    const std::vector<SpirvData>& getSpirvShaders() const;

    VkPipelineLayout getPipelineLayout() const;

};

#endif /* ENGINE_RENDERER_MATERIAL_MATERIALSHADER_H_ */
