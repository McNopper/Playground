#ifndef ENGINE_RENDERER_MATERIAL_TEXTURE2DSAMPLER_H_
#define ENGINE_RENDERER_MATERIAL_TEXTURE2DSAMPLER_H_

#include <volk.h>

// Forward declarations
class Texture2D;
class Sampler;

// Texture2DSampler - Convenience wrapper pairing Texture2D + Sampler
// Holds references only - does not own resources (they may be shared)
// Common pattern in material systems where textures are typically paired with samplers
// Inspired by MaterialX texture nodes and OpenUSD UsdShadeShader conventions
class Texture2DSampler
{

private:

    const Texture2D* m_texture{ nullptr };
    const Sampler* m_sampler{ nullptr };

public:

    Texture2DSampler() = default;

    Texture2DSampler(
        const Texture2D* texture,
        const Sampler* sampler
    );

    ~Texture2DSampler() = default;

    // Getters
    const Texture2D* getTexture() const;
    const Sampler* getSampler() const;

    // Setters
    void setTexture(const Texture2D* texture);
    void setSampler(const Sampler* sampler);

    // Convenience factory methods with common sampler configurations
    // Note: Caller must manage lifetime of returned sampler
    static Sampler* createLinearSampler(VkDevice device);
    static Sampler* createNearestSampler(VkDevice device);
    static Sampler* createLinearClampSampler(VkDevice device);

};

#endif /* ENGINE_RENDERER_MATERIAL_TEXTURE2DSAMPLER_H_ */
