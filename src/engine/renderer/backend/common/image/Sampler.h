#ifndef ENGINE_RENDERER_BACKEND_COMMON_IMAGE_SAMPLER_H_
#define ENGINE_RENDERER_BACKEND_COMMON_IMAGE_SAMPLER_H_

#include <volk.h>

/**
 * Sampler - Vulkan sampler wrapper
 * 
 * Wraps VkSampler for texture sampling configuration.
 * Provides filtering and addressing mode control.
 */

class Sampler
{

protected:

    VkDevice m_device{ VK_NULL_HANDLE };
    VkSampler m_sampler{ VK_NULL_HANDLE };

    VkFilter m_mag_filter{ VK_FILTER_LINEAR };
    VkFilter m_min_filter{ VK_FILTER_LINEAR };
    VkSamplerMipmapMode m_mipmap_mode{ VK_SAMPLER_MIPMAP_MODE_LINEAR };
    VkSamplerAddressMode m_address_mode_u{ VK_SAMPLER_ADDRESS_MODE_REPEAT };
    VkSamplerAddressMode m_address_mode_v{ VK_SAMPLER_ADDRESS_MODE_REPEAT };
    VkSamplerAddressMode m_address_mode_w{ VK_SAMPLER_ADDRESS_MODE_REPEAT };
    float m_max_anisotropy{ 0.0f };
    float m_min_lod{ 0.0f };
    float m_max_lod{ VK_LOD_CLAMP_NONE };

public:

    Sampler() = delete;
    Sampler(const Sampler& other) = delete;

    explicit Sampler(VkDevice device);

    virtual ~Sampler();

    Sampler operator=(const Sampler& other) = delete;

    void setMagFilter(VkFilter filter);
    void setMinFilter(VkFilter filter);
    void setAddressMode(VkSamplerAddressMode mode);
    void setAddressModeU(VkSamplerAddressMode mode);
    void setAddressModeV(VkSamplerAddressMode mode);
    void setAddressModeW(VkSamplerAddressMode mode);

    void setMaxAnisotropy(float max_anisotropy);

    void setMipmapMode(VkSamplerMipmapMode mode);

    void setMinLod(float min_lod);

    void setMaxLod(float max_lod);

    bool create();

    VkSampler getSampler() const;
    VkFilter getMagFilter() const;
    VkFilter getMinFilter() const;
    VkSamplerAddressMode getAddressModeU() const;
    VkSamplerAddressMode getAddressModeV() const;
    VkSamplerAddressMode getAddressModeW() const;

    bool isValid() const;

    void destroy();
};

#endif /* ENGINE_RENDERER_BACKEND_COMMON_IMAGE_SAMPLER_H_ */
