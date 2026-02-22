#ifndef GPU_VULKAN_FACTORY_VULKANSAMPLERFACTORY_H_
#define GPU_VULKAN_FACTORY_VULKANSAMPLERFACTORY_H_

#include <volk.h>

class VulkanSamplerFactory
{

private:

    VkDevice m_device{ VK_NULL_HANDLE };

    VkSamplerCreateFlags m_flags{ 0u };
    VkFilter m_mag_filter{ VK_FILTER_NEAREST };
    VkFilter m_min_filter{ VK_FILTER_NEAREST };
    VkSamplerMipmapMode m_mipmap_mode{ VK_SAMPLER_MIPMAP_MODE_NEAREST };
    VkSamplerAddressMode m_address_mode_u{ VK_SAMPLER_ADDRESS_MODE_REPEAT };
    VkSamplerAddressMode m_address_mode_v{ VK_SAMPLER_ADDRESS_MODE_REPEAT };
    VkSamplerAddressMode m_address_mode_w{ VK_SAMPLER_ADDRESS_MODE_REPEAT };
    float m_mip_lod_bias{ 0.0f };
    VkBool32 m_anisotropy_enable{ VK_FALSE };
    float m_max_anisotropy{ 1.0f };
    VkBool32 m_compare_enable{ VK_FALSE };
    VkCompareOp m_compare_op{ VK_COMPARE_OP_NEVER };
    float m_min_lod{ 0.0f };
    float m_max_lod{ 0.0f };
    VkBorderColor m_border_color{ VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK };
    VkBool32 m_unnormalized_coordinates{ VK_FALSE };

public:

    VulkanSamplerFactory() = delete;

    VulkanSamplerFactory(VkDevice device, VkSamplerCreateFlags flags);

    void setMagFilter(VkFilter filter);

    void setMinFilter(VkFilter filter);

    void setSamplerMipmapMode(VkSamplerMipmapMode mode);

    void setSamplerAddressModeU(VkSamplerAddressMode mode);

    void setSamplerAddressModeV(VkSamplerAddressMode mode);

    void setSamplerAddressModeW(VkSamplerAddressMode mode);

    void setMaxAnisotropy(float max_anisotropy);

    VkSampler create() const;
};

#endif /* GPU_VULKAN_FACTORY_VULKANSAMPLERFACTORY_H_ */
