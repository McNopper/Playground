#include "VulkanSamplerFactory.h"

VulkanSamplerFactory::VulkanSamplerFactory(VkDevice device, VkSamplerCreateFlags flags) :
    m_device{ device }, m_flags{ flags }
{
}

void VulkanSamplerFactory::setMagFilter(VkFilter filter)
{
    m_mag_filter = filter;
}

void VulkanSamplerFactory::setMinFilter(VkFilter filter)
{
    m_min_filter = filter;
}

void VulkanSamplerFactory::setSamplerMipmapMode(VkSamplerMipmapMode mode)
{
    m_mipmap_mode = mode;
}

void VulkanSamplerFactory::setSamplerAddressModeU(VkSamplerAddressMode mode)
{
    m_address_mode_u = mode;
}

void VulkanSamplerFactory::setSamplerAddressModeV(VkSamplerAddressMode mode)
{
    m_address_mode_v = mode;
}

void VulkanSamplerFactory::setSamplerAddressModeW(VkSamplerAddressMode mode)
{
    m_address_mode_w = mode;
}

void VulkanSamplerFactory::setMaxAnisotropy(float max_anisotropy)
{
    m_anisotropy_enable = VK_TRUE;
    m_max_anisotropy = max_anisotropy;
}

VkSampler VulkanSamplerFactory::create() const
{
    VkSamplerCreateInfo sampler_create_info{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
    sampler_create_info.flags = m_flags;
    sampler_create_info.magFilter = m_mag_filter;
    sampler_create_info.minFilter = m_min_filter;
    sampler_create_info.mipmapMode = m_mipmap_mode;
    sampler_create_info.addressModeU = m_address_mode_u;
    sampler_create_info.addressModeV = m_address_mode_v;
    sampler_create_info.addressModeW = m_address_mode_w;
    sampler_create_info.mipLodBias = m_mip_lod_bias;
    sampler_create_info.anisotropyEnable = m_anisotropy_enable;
    sampler_create_info.maxAnisotropy = m_max_anisotropy;
    sampler_create_info.compareEnable = m_compare_enable;
    sampler_create_info.compareOp = m_compare_op;
    sampler_create_info.minLod = m_min_lod;
    sampler_create_info.maxLod = m_max_lod;
    sampler_create_info.borderColor = m_border_color;
    sampler_create_info.unnormalizedCoordinates = m_unnormalized_coordinates;

    VkSampler sampler{ VK_NULL_HANDLE };
    vkCreateSampler(m_device, &sampler_create_info, nullptr, &sampler);

    return sampler;
}
