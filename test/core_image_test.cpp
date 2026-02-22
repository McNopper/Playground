#include <gtest/gtest.h>

#include "core/core.h"

TEST(TestImage, LoadSavePNG)
{
    auto image_data = loadImageData("../resources/images/color_grid.png");

    EXPECT_TRUE(image_data.has_value());

    if (image_data.has_value())
    {
        auto result = saveImageData("../bin/color_grid.png", *image_data);

        EXPECT_TRUE(result);
    }
}

TEST(TestImage, LoadSaveEXR)
{
    auto image_data = loadImageData("../resources/images/color_grid.exr");

    EXPECT_TRUE(image_data.has_value());

    if (image_data.has_value())
    {
        auto result = saveImageData("../bin/color_grid.exr", *image_data);

        EXPECT_TRUE(result);
    }
}

TEST(TestImage, ConvertChannelAddAlpha)
{
    auto image_data = loadImageData("../resources/images/red.png");

    EXPECT_TRUE(image_data.has_value());

    if (image_data.has_value())
    {
        auto converted_image_data = convertImageDataChannels(4, *image_data);

        EXPECT_TRUE(converted_image_data.has_value());

        if (converted_image_data.has_value())
        {
            auto result = saveImageData("../bin/red_alpha.png", *converted_image_data);

            EXPECT_TRUE(result);
        }
    }
}

TEST(TestImage, ConvertColorSpaceToLinear)
{
    auto image_data = loadImageData("../resources/images/color_grid.png");

    EXPECT_TRUE(image_data.has_value());

    if (image_data.has_value())
    {
        auto converted_image_data = convertImageDataColorSpace(ColorSpace::SRGB, true, *image_data);

        EXPECT_TRUE(converted_image_data.has_value());

        if (converted_image_data.has_value())
        {
            auto result = saveImageData("../bin/color_grid_linear.png", *converted_image_data);

            EXPECT_TRUE(result);
        }
    }
}

TEST(TestImage, GenerateMipMapsCount)
{
    // 256x256 should produce 9 levels: 256, 128, 64, 32, 16, 8, 4, 2, 1
    ImageData image_data{};
    image_data.width = 256u;
    image_data.height = 256u;
    image_data.channels = 4u;
    image_data.channel_format = ChannelFormat::UNORM;
    image_data.color_space = ColorSpace::SRGB;
    image_data.linear = true;
    image_data.pixels.resize(256u * 256u * 4u, 128u);

    auto mip_levels = generateMipMaps(image_data);

    EXPECT_EQ(mip_levels.size(), 9u);
}

TEST(TestImage, GenerateMipMapsDimensions)
{
    ImageData image_data{};
    image_data.width = 256u;
    image_data.height = 128u;
    image_data.channels = 4u;
    image_data.channel_format = ChannelFormat::UNORM;
    image_data.color_space = ColorSpace::SRGB;
    image_data.linear = true;
    image_data.pixels.resize(256u * 128u * 4u, 200u);

    auto mip_levels = generateMipMaps(image_data);

    // Level 0: 256x128, level 1: 128x64, ..., last: 1x1
    ASSERT_FALSE(mip_levels.empty());
    EXPECT_EQ(mip_levels[0].width, 256u);
    EXPECT_EQ(mip_levels[0].height, 128u);
    EXPECT_EQ(mip_levels[1].width, 128u);
    EXPECT_EQ(mip_levels[1].height, 64u);
    EXPECT_EQ(mip_levels[2].width, 64u);
    EXPECT_EQ(mip_levels[2].height, 32u);

    const ImageData& last = mip_levels.back();
    EXPECT_EQ(last.width, 1u);
    EXPECT_EQ(last.height, 1u);
}

TEST(TestImage, GenerateMipMapsPreservesMetadata)
{
    ImageData image_data{};
    image_data.width = 64u;
    image_data.height = 64u;
    image_data.channels = 3u;
    image_data.channel_format = ChannelFormat::SFLOAT;
    image_data.color_space = ColorSpace::BT709;
    image_data.linear = true;
    image_data.pixels.resize(64u * 64u * 3u * 4u, 0u); // 4 bytes per SFLOAT channel

    auto mip_levels = generateMipMaps(image_data);

    for (const auto& mip : mip_levels)
    {
        EXPECT_EQ(mip.channels, 3u);
        EXPECT_EQ(mip.channel_format, ChannelFormat::SFLOAT);
        EXPECT_EQ(mip.color_space, ColorSpace::BT709);
        EXPECT_TRUE(mip.linear);
    }
}

TEST(TestImage, GenerateMipMapsPixelBufferSize)
{
    ImageData image_data{};
    image_data.width = 64u;
    image_data.height = 64u;
    image_data.channels = 4u;
    image_data.channel_format = ChannelFormat::UNORM;
    image_data.color_space = ColorSpace::SRGB;
    image_data.linear = true;
    image_data.pixels.resize(64u * 64u * 4u, 255u);

    auto mip_levels = generateMipMaps(image_data);

    for (const auto& mip : mip_levels)
    {
        std::size_t expected_size = mip.width * mip.height * mip.channels * getChannelFormatSize(mip.channel_format);
        EXPECT_EQ(mip.pixels.size(), expected_size);
    }
}

TEST(TestImage, GenerateMipMapsFromFile)
{
    auto image_data = loadImageData("../resources/images/color_grid.png");
    ASSERT_TRUE(image_data.has_value());

    const ImageData& base = *image_data;
    auto mip_levels = generateMipMaps(base);

    EXPECT_GT(mip_levels.size(), 1u);
    EXPECT_EQ(mip_levels[0].width, base.width);
    EXPECT_EQ(mip_levels[0].height, base.height);

    const ImageData& last = mip_levels.back();
    EXPECT_EQ(last.width, 1u);
    EXPECT_EQ(last.height, 1u);

    // Save the smallest mip to verify pixel data was written
    EXPECT_FALSE(last.pixels.empty());
}
