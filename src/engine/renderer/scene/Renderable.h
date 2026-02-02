#ifndef ENGINE_RENDERER_SCENE_RENDERABLE_H_
#define ENGINE_RENDERER_SCENE_RENDERABLE_H_

#include <memory>

#include <volk.h>

#include "core/math/matrix.h"

// Forward declarations
class AGeometry;
class MaterialShader;
class UniformBuffer;

// Renderable combines geometry, material, and world transform
// Represents a single object that can be rendered in the scene
// Owns and manages per-object uniform buffer for world transform
// Inspired by ANARI's Surface+Instance concept and OpenUSD's Gprim+Material binding
class Renderable
{

private:

    VkPhysicalDevice m_physical_device{ VK_NULL_HANDLE };
    VkDevice m_device{ VK_NULL_HANDLE };

    std::shared_ptr<AGeometry> m_geometry{};
    std::shared_ptr<MaterialShader> m_material{};
    std::unique_ptr<UniformBuffer> m_uniform_model_buffer{};
    float4x4 m_transform{};

public:

    Renderable() = delete;

    Renderable(
        VkPhysicalDevice physical_device,
        VkDevice device,
        std::shared_ptr<AGeometry> geometry,
        std::shared_ptr<MaterialShader> material,
        const float4x4& transform = float4x4(1.0f)
    );

    ~Renderable();

    // Initialize uniform buffer and bind to material
    bool init();

    // Getters
    const std::shared_ptr<AGeometry>& getGeometry() const;
    const std::shared_ptr<MaterialShader>& getMaterial() const;
    UniformBuffer* getUniformModelBuffer() const;
    const float4x4& getTransform() const;

    // Setters
    void setGeometry(std::shared_ptr<AGeometry> geometry);
    void setMaterial(std::shared_ptr<MaterialShader> material);
    void setTransform(const float4x4& transform);

};

#endif /* ENGINE_RENDERER_SCENE_RENDERABLE_H_ */
