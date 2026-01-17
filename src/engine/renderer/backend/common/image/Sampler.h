#ifndef RENDERER_BACKEND_COMMON_IMAGE_SAMPLER_H_
#define RENDERER_BACKEND_COMMON_IMAGE_SAMPLER_H_

#include <volk.h>

/**
 * Sampler - Vulkan sampler wrapper
 * 
 * Wraps VkSampler for texture sampling configuration.
 * Provides filtering and addressing mode control.
 */

class Sampler {

protected:

	VkDevice m_device{ VK_NULL_HANDLE };
	VkSampler m_sampler{ VK_NULL_HANDLE };

	VkFilter m_mag_filter{ VK_FILTER_LINEAR };
	VkFilter m_min_filter{ VK_FILTER_LINEAR };
	VkSamplerAddressMode m_address_mode_u{ VK_SAMPLER_ADDRESS_MODE_REPEAT };
	VkSamplerAddressMode m_address_mode_v{ VK_SAMPLER_ADDRESS_MODE_REPEAT };
	VkSamplerAddressMode m_address_mode_w{ VK_SAMPLER_ADDRESS_MODE_REPEAT };

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

#endif /* RENDERER_BACKEND_COMMON_IMAGE_SAMPLER_H_ */
