#include "Sampler.h"

#include "gpu/gpu.h"

Sampler::Sampler(VkDevice device) :
    m_device{ device }
{
}

Sampler::~Sampler()
{
    destroy();
}

void Sampler::setMagFilter(VkFilter filter)
{
    m_mag_filter = filter;
}

void Sampler::setMinFilter(VkFilter filter)
{
    m_min_filter = filter;
}

void Sampler::setAddressMode(VkSamplerAddressMode mode)
{
    m_address_mode_u = mode;
    m_address_mode_v = mode;
    m_address_mode_w = mode;
}

void Sampler::setAddressModeU(VkSamplerAddressMode mode)
{
    m_address_mode_u = mode;
}

void Sampler::setAddressModeV(VkSamplerAddressMode mode)
{
    m_address_mode_v = mode;
}

void Sampler::setAddressModeW(VkSamplerAddressMode mode)
{
    m_address_mode_w = mode;
}

void Sampler::setMaxAnisotropy(float max_anisotropy)
{
    m_max_anisotropy = max_anisotropy;
}

void Sampler::setMipmapMode(VkSamplerMipmapMode mode)
{
    m_mipmap_mode = mode;
}

void Sampler::setMinLod(float min_lod)
{
    m_min_lod = min_lod;
}

void Sampler::setMaxLod(float max_lod)
{
    m_max_lod = max_lod;
}

bool Sampler::create()
{
    destroy();

    VulkanSamplerFactory sampler_factory{ m_device, 0u };
    sampler_factory.setMagFilter(m_mag_filter);
    sampler_factory.setMinFilter(m_min_filter);
    sampler_factory.setSamplerMipmapMode(m_mipmap_mode);
    sampler_factory.setSamplerAddressModeU(m_address_mode_u);
    sampler_factory.setSamplerAddressModeV(m_address_mode_v);
    sampler_factory.setSamplerAddressModeW(m_address_mode_w);
    sampler_factory.setMinLod(m_min_lod);
    sampler_factory.setMaxLod(m_max_lod);
    if (m_max_anisotropy > 0.0f)
    {
        sampler_factory.setMaxAnisotropy(m_max_anisotropy);
    }

    m_sampler = sampler_factory.create();

    return m_sampler != VK_NULL_HANDLE;
}

VkSampler Sampler::getSampler() const
{
    return m_sampler;
}

VkFilter Sampler::getMagFilter() const
{
    return m_mag_filter;
}

VkFilter Sampler::getMinFilter() const
{
    return m_min_filter;
}

VkSamplerAddressMode Sampler::getAddressModeU() const
{
    return m_address_mode_u;
}

VkSamplerAddressMode Sampler::getAddressModeV() const
{
    return m_address_mode_v;
}

VkSamplerAddressMode Sampler::getAddressModeW() const
{
    return m_address_mode_w;
}

bool Sampler::isValid() const
{
    return m_sampler != VK_NULL_HANDLE;
}

void Sampler::destroy()
{
    if (m_sampler != VK_NULL_HANDLE)
    {
        vkDestroySampler(m_device, m_sampler, nullptr);
        m_sampler = VK_NULL_HANDLE;
    }
}
