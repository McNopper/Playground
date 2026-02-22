#ifndef CORE_IMAGE_DATA_H_
#define CORE_IMAGE_DATA_H_

#include <cstdint>
#include <optional>
#include <vector>

#include "core/color/types.h"

// Image coordinate system convention:
// - Images use top-left origin throughout: files, memory, and GPU textures
// - UV coordinates: (0,0) = top-left, (1,1) = bottom-right
// - GPU texture sampling: V=0 samples first row (top of image)
// - File format: First row = top of image (PNG/JPEG standard)
// - Everything is consistent - no flipping needed anywhere

enum class ChannelFormat
{
    UNDEFINED,
    UNORM,
    SHALF,
    SFLOAT
};

struct ImageData
{
    uint32_t width{ 0u };
    uint32_t height{ 0u };

    uint32_t channels{ 0u };
    ChannelFormat channel_format{ ChannelFormat::UNDEFINED };

    ColorSpace color_space{ ColorSpace::UNKNOWN };
    bool linear{ false };

    std::vector<uint8_t> pixels{};
};

uint32_t getChannelFormatSize(ChannelFormat channel_format);

std::optional<ImageData> loadImageData(const char* filename);

bool saveImageData(const char* filename, const ImageData& image_data);

std::optional<ImageData> convertImageDataChannels(uint32_t channels, const ImageData& image_data);

std::optional<ImageData> convertImageDataColorSpace(ColorSpace color_space, bool linear, const ImageData& image_data);

std::vector<ImageData> generateMipMaps(const ImageData& image_data);

#endif /* CORE_IMAGE_DATA_H_ */
