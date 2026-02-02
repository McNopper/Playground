#include "Texture2DSampler.h"

#include "engine/renderer/backend/common/image/Texture2D.h"
#include "engine/renderer/backend/common/image/Sampler.h"

Texture2DSampler::Texture2DSampler(
    const Texture2D* texture,
    const Sampler* sampler
) :
    m_texture{ texture }, m_sampler{ sampler }
{
}

const Texture2D* Texture2DSampler::getTexture() const
{
    return m_texture;
}

const Sampler* Texture2DSampler::getSampler() const
{
    return m_sampler;
}

void Texture2DSampler::setTexture(const Texture2D* texture)
{
    m_texture = texture;
}

void Texture2DSampler::setSampler(const Sampler* sampler)
{
    m_sampler = sampler;
}

Sampler* Texture2DSampler::createLinearSampler(VkDevice device)
{
    auto* sampler = new Sampler(device);
    sampler->setMagFilter(VK_FILTER_LINEAR);
    sampler->setMinFilter(VK_FILTER_LINEAR);
    sampler->setAddressMode(VK_SAMPLER_ADDRESS_MODE_REPEAT);
    
    if (!sampler->create())
    {
        delete sampler;
        return nullptr;
    }

    return sampler;
}

Sampler* Texture2DSampler::createNearestSampler(VkDevice device)
{
    auto* sampler = new Sampler(device);
    sampler->setMagFilter(VK_FILTER_NEAREST);
    sampler->setMinFilter(VK_FILTER_NEAREST);
    sampler->setAddressMode(VK_SAMPLER_ADDRESS_MODE_REPEAT);
    
    if (!sampler->create())
    {
        delete sampler;
        return nullptr;
    }

    return sampler;
}

Sampler* Texture2DSampler::createLinearClampSampler(VkDevice device)
{
    auto* sampler = new Sampler(device);
    sampler->setMagFilter(VK_FILTER_LINEAR);
    sampler->setMinFilter(VK_FILTER_LINEAR);
    sampler->setAddressMode(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
    
    if (!sampler->create())
    {
        delete sampler;
        return nullptr;
    }

    return sampler;
}
