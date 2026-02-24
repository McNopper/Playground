#ifndef ENGINE_RENDERER_BACKEND_COMMON_IMAGE_TEXTURECUBE_H_
#define ENGINE_RENDERER_BACKEND_COMMON_IMAGE_TEXTURECUBE_H_

#include "Texture.h"

/**
 * TextureCube - Cube map texture (6 faces, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT)
 *
 * All faces are square (width == height). Provides a VK_IMAGE_VIEW_TYPE_CUBE
 * view for sampling and a VK_IMAGE_VIEW_TYPE_2D_ARRAY view for compute
 * shader storage writes (e.g. IBL precomputation).
 */

class TextureCube : public Texture
{

private:

    std::vector<VkImageView> m_mip_storage_image_views{};

public:

    TextureCube() = delete;
    TextureCube(const TextureCube& other) = delete;

    TextureCube(VkPhysicalDevice physical_device, VkDevice device);

    ~TextureCube() override;

    TextureCube operator=(const TextureCube& other) = delete;

    void setSize(uint32_t size);

    bool create() override;

    VkImageView getStorageImageView() const;

    VkImageView getStorageImageViewForMip(uint32_t mip) const;

    void destroy() override;

    uint32_t getSize() const;
};

#endif /* ENGINE_RENDERER_BACKEND_COMMON_IMAGE_TEXTURECUBE_H_ */
