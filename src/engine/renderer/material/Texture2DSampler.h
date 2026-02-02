#ifndef ENGINE_RENDERER_MATERIAL_TEXTURE2DSAMPLER_H_
#define ENGINE_RENDERER_MATERIAL_TEXTURE2DSAMPLER_H_

#include <memory>

#include <volk.h>

// Forward declarations
class Texture2D;
class Sampler;

// Texture2DSampler - Convenience wrapper pairing Texture2D + Sampler
// Holds shared references - participates in resource lifetime management
// Common pattern in material systems where textures are typically paired with samplers
// Inspired by MaterialX texture nodes and OpenUSD UsdShadeShader conventions
class Texture2DSampler
{

private:

    std::shared_ptr<const Texture2D> m_texture{};
    std::shared_ptr<const Sampler> m_sampler{};

public:

    Texture2DSampler() = default;

    Texture2DSampler(
        const std::shared_ptr<const Texture2D>& texture,
        const std::shared_ptr<const Sampler>& sampler
    );

    ~Texture2DSampler() = default;

    // Getters
    std::shared_ptr<const Texture2D> getTexture() const;
    std::shared_ptr<const Sampler> getSampler() const;

    // Setters
    void setTexture(const std::shared_ptr<const Texture2D>& texture);
    void setSampler(const std::shared_ptr<const Sampler>& sampler);

    // Convenience factory methods with common sampler configurations
    // Returns unique_ptr for automatic lifetime management
    static std::unique_ptr<Sampler> createLinearSampler(VkDevice device);
    static std::unique_ptr<Sampler> createNearestSampler(VkDevice device);
    static std::unique_ptr<Sampler> createLinearClampSampler(VkDevice device);

};

#endif /* ENGINE_RENDERER_MATERIAL_TEXTURE2DSAMPLER_H_ */
