#ifndef RENDERER_BACKEND_COMMON_IMAGE_TEXTURE2D_H_
#define RENDERER_BACKEND_COMMON_IMAGE_TEXTURE2D_H_

#include "Texture.h"

/**
 * Texture2D - 2D texture (VK_IMAGE_TYPE_2D)
 * 
 * Two-dimensional texture with width and height.
 * Most common texture type, supports ImageData loading.
 */

class Texture2D : public Texture {

public:

	Texture2D() = delete;
	Texture2D(const Texture2D& other) = delete;

	Texture2D(VkPhysicalDevice physical_device, VkDevice device);

	~Texture2D() override;

	Texture2D operator=(const Texture2D& other) = delete;

	void setWidth(uint32_t width);
	void setHeight(uint32_t height);
	void setExtent(uint32_t width, uint32_t height);

	bool create() override;

	bool upload(VkCommandPool command_pool, VkQueue queue, const ImageData& image_data, uint32_t mip_level = 0);

	bool uploadMipMaps(VkCommandPool command_pool, VkQueue queue, const std::vector<ImageData>& mip_levels);

	uint32_t getWidth() const;
	uint32_t getHeight() const;

};

#endif /* RENDERER_BACKEND_COMMON_IMAGE_TEXTURE2D_H_ */
