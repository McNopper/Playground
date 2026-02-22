#include "Texture2DSampler.h"

#include <memory>

#include "engine/renderer/backend/common/image/Sampler.h"
#include "engine/renderer/backend/common/image/Texture2D.h"

Texture2DSampler::Texture2DSampler(
    const std::shared_ptr<const Texture2D>& texture,
    const std::shared_ptr<const Sampler>& sampler) :
    m_texture{ texture },
    m_sampler{ sampler }
{
}

std::shared_ptr<const Texture2D> Texture2DSampler::getTexture() const
{
    return m_texture;
}

std::shared_ptr<const Sampler> Texture2DSampler::getSampler() const
{
    return m_sampler;
}

void Texture2DSampler::setTexture(const std::shared_ptr<const Texture2D>& texture)
{
    m_texture = texture;
}

void Texture2DSampler::setSampler(const std::shared_ptr<const Sampler>& sampler)
{
    m_sampler = sampler;
}

std::unique_ptr<Sampler> Texture2DSampler::createLinearSampler(VkDevice device)
{
    auto sampler = std::make_unique<Sampler>(device);
    sampler->setMagFilter(VK_FILTER_LINEAR);
    sampler->setMinFilter(VK_FILTER_LINEAR);
    sampler->setAddressMode(VK_SAMPLER_ADDRESS_MODE_REPEAT);

    if (!sampler->create())
    {
        return nullptr;
    }

    return sampler;
}

std::unique_ptr<Sampler> Texture2DSampler::createNearestSampler(VkDevice device)
{
    auto sampler = std::make_unique<Sampler>(device);
    sampler->setMagFilter(VK_FILTER_NEAREST);
    sampler->setMinFilter(VK_FILTER_NEAREST);
    sampler->setAddressMode(VK_SAMPLER_ADDRESS_MODE_REPEAT);

    if (!sampler->create())
    {
        return nullptr;
    }

    return sampler;
}

std::unique_ptr<Sampler> Texture2DSampler::createLinearClampSampler(VkDevice device)
{
    auto sampler = std::make_unique<Sampler>(device);
    sampler->setMagFilter(VK_FILTER_LINEAR);
    sampler->setMinFilter(VK_FILTER_LINEAR);
    sampler->setAddressMode(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);

    if (!sampler->create())
    {
        return nullptr;
    }

    return sampler;
}
