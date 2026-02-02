#ifndef ENGINE_RENDERER_SCENE_RENDERABLE_H_
#define ENGINE_RENDERER_SCENE_RENDERABLE_H_

#include <memory>

#include <volk.h>

#include "core/math/matrix.h"

// Forward declarations
class AGeometry;
class MaterialShader;
class UniformBuffer;
class UniformBlock;

// Renderable combines geometry, material, and world transform
// Represents a single object that can be rendered in the scene
// Owns and manages per-object uniform buffer (typically for world transform + custom data)
// Exposes UniformBlock for external updates before calling updateUniforms()
// Inspired by ANARI's Surface+Instance concept and OpenUSD's Gprim+Material binding
class Renderable
{

private:

    VkPhysicalDevice m_physical_device{ VK_NULL_HANDLE };
    VkDevice m_device{ VK_NULL_HANDLE };

    std::shared_ptr<AGeometry> m_geometry{};
    std::shared_ptr<MaterialShader> m_material{};
    std::shared_ptr<UniformBuffer> m_uniform_model_buffer{};
    std::shared_ptr<UniformBlock> m_uniform_model_block{};
    float4x4 m_world_matrix{};

public:

    Renderable() = delete;

    Renderable(
        VkPhysicalDevice physical_device,
        VkDevice device,
        std::shared_ptr<AGeometry> geometry,
        std::shared_ptr<MaterialShader> material,
        const float4x4& world_matrix = float4x4(1.0f)
    );

    ~Renderable();

    // Initialize uniform buffer and bind to material
    bool init();

    // Update uniform buffer with current transform
    bool updateUniforms();

    // Render the object (binds material and draws geometry)
    void render(VkCommandBuffer command_buffer) const;

    // Getters
    const float4x4& getWorldMatrix() const;
    std::shared_ptr<UniformBlock> getUniformBlock() const;

    // Setter for world matrix (can be updated per frame)
    void setWorldMatrix(const float4x4& world_matrix);

};

#endif /* ENGINE_RENDERER_SCENE_RENDERABLE_H_ */
