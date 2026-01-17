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

bool Sampler::create()
{
	destroy();

	VulkanSamplerFactory sampler_factory{ m_device, 0u };
	sampler_factory.setMagFilter(m_mag_filter);
	sampler_factory.setMinFilter(m_min_filter);
	sampler_factory.setSamplerAddressModeU(m_address_mode_u);
	sampler_factory.setSamplerAddressModeV(m_address_mode_v);
	sampler_factory.setSamplerAddressModeW(m_address_mode_w);

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
