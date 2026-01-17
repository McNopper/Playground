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
