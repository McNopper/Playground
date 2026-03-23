#include "image_data.h"

#include <algorithm>
#include <cstring>

#include <OpenImageIO/imagebuf.h>
#include <OpenImageIO/imagebufalgo.h>
#include <OpenImageIO/imageio.h>
#include <OpenImageIO/span.h>

#include "core/color/convert.h"
#include "core/math/matrix.h"

ChannelFormat getChannelFormat(OIIO::TypeDesc format)
{
    if (format == OIIO::TypeDesc::UINT8)
    {
        return ChannelFormat::UNORM;
    }
    if (format == OIIO::TypeDesc::HALF)
    {
        return ChannelFormat::SHALF;
    }
    if (format == OIIO::TypeDesc::FLOAT)
    {
        return ChannelFormat::SFLOAT;
    }

    return ChannelFormat::UNDEFINED;
}

OIIO::TypeDesc getChannelFormat(ChannelFormat channel_format)
{
    if (channel_format == ChannelFormat::UNORM)
    {
        return OIIO::TypeDesc::UINT8;
    }
    if (channel_format == ChannelFormat::SHALF)
    {
        return OIIO::TypeDesc::HALF;
    }
    if (channel_format == ChannelFormat::SFLOAT)
    {
        return OIIO::TypeDesc::FLOAT;
    }

    return OIIO::TypeDesc::UNKNOWN;
}

//

uint32_t getChannelFormatSize(ChannelFormat channel_format)
{
    if (channel_format == ChannelFormat::UNORM)
    {
        return 1u;
    }
    if (channel_format == ChannelFormat::SHALF)
    {
        return 2u;
    }
    if (channel_format == ChannelFormat::SFLOAT)
    {
        return 4u;
    }

    return 0u;
}

std::optional<ImageData> loadImageData(const char* filename)
{
    auto image_input = OIIO::ImageInput::open(filename);
    if (!image_input)
    {
        return {};
    }

    const OIIO::ImageSpec& image_spec = image_input->spec();
    const OIIO::ParamValue* color_space_parameter = image_spec.find_attribute("oiio:ColorSpace", OIIO::TypeDesc::STRING);
    if (!color_space_parameter)
    {
        return {};
    }
    std::string color_space = color_space_parameter->get_string();
    std::transform(color_space.begin(), color_space.end(), color_space.begin(), ::tolower);

    ImageData image_data{};

    image_data.width = (uint32_t)image_spec.width;
    image_data.height = (uint32_t)image_spec.height;

    image_data.channels = (uint32_t)image_spec.nchannels;
    image_data.channel_format = getChannelFormat(image_spec.format);
    uint32_t channel_size = getChannelFormatSize(image_data.channel_format);
    if (image_data.channel_format == ChannelFormat::UNDEFINED || channel_size == 0u)
    {
        return {};
    }

    if (color_space == "linear" || color_space == "lin_srgb" || color_space == "scene_linear" ||
        color_space == "lin_rec709" || color_space == "lin_rec709_scene")
    {
        image_data.primaries = ColorPrimaries::REC709;
        image_data.transfer = TransferFunction::LINEAR;
        image_data.image_state = ImageState::SCENE;
    }
    else if (color_space == "srgb" || color_space == "srgb_rec709_scene")
    {
        image_data.primaries = ColorPrimaries::REC709;
        image_data.transfer = TransferFunction::SRGB;
        image_data.image_state = ImageState::SCENE;
    }
    else if (color_space == "scrgb")
    {
        image_data.primaries = ColorPrimaries::REC709;
        image_data.transfer = TransferFunction::EXTENDED_SRGB;
        image_data.image_state = ImageState::DISPLAY;
    }
    else if (color_space == "rec709" || color_space == "g22_rec709_scene")
    {
        image_data.primaries = ColorPrimaries::REC709;
        image_data.transfer = TransferFunction::BT709;
        image_data.image_state = ImageState::SCENE;
    }
    else if (color_space == "lin_rec2020" || color_space == "lin_rec2020_scene" || color_space == "rec2020")
    {
        image_data.primaries = ColorPrimaries::REC2020;
        image_data.transfer = TransferFunction::LINEAR;
        image_data.image_state = ImageState::SCENE;
    }
    else if (color_space == "pq_rec2020_display")
    {
        image_data.primaries = ColorPrimaries::REC2020;
        image_data.transfer = TransferFunction::ST2084_PQ;
        image_data.image_state = ImageState::DISPLAY;
    }
    else if (color_space == "hlg_rec2020_display")
    {
        image_data.primaries = ColorPrimaries::REC2020;
        image_data.transfer = TransferFunction::HLG;
        image_data.image_state = ImageState::DISPLAY;
    }
    else if (color_space == "srgb_rec709_display")
    {
        image_data.primaries = ColorPrimaries::REC709;
        image_data.transfer = TransferFunction::SRGB;
        image_data.image_state = ImageState::DISPLAY;
    }
    else if (color_space == "g24_rec709_display")
    {
        image_data.primaries = ColorPrimaries::REC709;
        image_data.transfer = TransferFunction::BT709;
        image_data.image_state = ImageState::DISPLAY;
    }
    else if (color_space == "lin_rec709_display")
    {
        image_data.primaries = ColorPrimaries::REC709;
        image_data.transfer = TransferFunction::LINEAR;
        image_data.image_state = ImageState::DISPLAY;
    }
    else if (color_space == "lin_rec2020_display")
    {
        image_data.primaries = ColorPrimaries::REC2020;
        image_data.transfer = TransferFunction::LINEAR;
        image_data.image_state = ImageState::DISPLAY;
    }
    else
    {
        return {};
    }

    image_data.pixels.resize(image_data.width * image_data.height * image_data.channels * channel_size);

    image_input->read_image(0, 0, 0, image_data.channels, image_spec.format, image_data.pixels.data());
    image_input->close();

    // Note: OpenImageIO loads images with top-left origin (standard for PNG/JPEG).
    // This matches Vulkan/glTF convention where UV (0,0) = top-left.
    // GPU texture sampling: V=0 samples first row, which is top of image.
    // No flipping needed - everything is already consistent.

    return image_data;
}

bool saveImageData(const char* filename, const ImageData& image_data)
{
    OIIO::TypeDesc format = getChannelFormat(image_data.channel_format);
    if (format == OIIO::TypeDesc::UNKNOWN)
    {
        return false;
    }

    std::unique_ptr<OIIO::ImageOutput> image_output = OIIO::ImageOutput::create(filename);
    if (!image_output)
    {
        return false;
    }

    OIIO::ImageSpec image_spec{ (int)image_data.width, (int)image_data.height, (int)image_data.channels, format };

    std::string color_space{};
    if (image_data.primaries == ColorPrimaries::REC709)
    {
        if (image_data.transfer == TransferFunction::LINEAR && image_data.image_state == ImageState::SCENE)
        {
            color_space = "lin_rec709_scene";
        }
        else if (image_data.transfer == TransferFunction::LINEAR && image_data.image_state == ImageState::DISPLAY)
        {
            color_space = "lin_rec709_display";
        }
        else if (image_data.transfer == TransferFunction::SRGB && image_data.image_state == ImageState::SCENE)
        {
            color_space = "srgb_rec709_scene";
        }
        else if (image_data.transfer == TransferFunction::SRGB && image_data.image_state == ImageState::DISPLAY)
        {
            color_space = "srgb_rec709_display";
        }
        else if (image_data.transfer == TransferFunction::BT709 && image_data.image_state == ImageState::SCENE)
        {
            color_space = "g22_rec709_scene";
        }
        else if (image_data.transfer == TransferFunction::BT709 && image_data.image_state == ImageState::DISPLAY)
        {
            color_space = "g24_rec709_display";
        }
        else if (image_data.transfer == TransferFunction::EXTENDED_SRGB)
        {
            color_space = "scRGB";
        }
    }
    else if (image_data.primaries == ColorPrimaries::REC2020)
    {
        if (image_data.transfer == TransferFunction::LINEAR && image_data.image_state == ImageState::SCENE)
        {
            color_space = "lin_rec2020_scene";
        }
        else if (image_data.transfer == TransferFunction::LINEAR && image_data.image_state == ImageState::DISPLAY)
        {
            color_space = "lin_rec2020_display";
        }
        else if (image_data.transfer == TransferFunction::ST2084_PQ)
        {
            color_space = "pq_rec2020_display";
        }
        else if (image_data.transfer == TransferFunction::HLG)
        {
            color_space = "hlg_rec2020_display";
        }
    }

    if (!color_space.empty())
    {
        image_spec.attribute("oiio:ColorSpace", color_space);
    }

    // Note: Internal storage and file format both use top-left origin.
    // No flipping needed - write directly.

    image_output->open(filename, image_spec);
    image_output->write_image(image_spec.format, image_data.pixels.data());
    image_output->close();

    return true;
}

std::optional<ImageData> convertImageDataChannels(uint32_t channels, const ImageData& image_data)
{
    if (channels < 1u || channels > 4u)
    {
        return {};
    }

    if (channels == image_data.channels)
    {
        return image_data;
    }

    OIIO::TypeDesc format = getChannelFormat(image_data.channel_format);
    if (format == OIIO::TypeDesc::UNKNOWN)
    {
        return {};
    }
    uint32_t channel_size = getChannelFormatSize(image_data.channel_format);

    OIIO::ImageSpec image_spec{ (int)image_data.width, (int)image_data.height, (int)image_data.channels, format };
    OIIO::ImageBuf image_buf(image_spec, (void*)image_data.pixels.data());

    ImageData converted_image_data{};
    converted_image_data.width = image_data.width;
    converted_image_data.height = image_data.height;
    converted_image_data.channels = channels;
    converted_image_data.channel_format = image_data.channel_format;
    converted_image_data.primaries = image_data.primaries;
    converted_image_data.transfer = image_data.transfer;
    converted_image_data.image_state = image_data.image_state;
    converted_image_data.pixels.resize(converted_image_data.width * converted_image_data.height * converted_image_data.channels * channel_size);

    OIIO::ImageSpec converted_image_spec{ (int)converted_image_data.width, (int)converted_image_data.height, (int)converted_image_data.channels, format };
    OIIO::ImageBuf converted_image_buf(converted_image_spec, (void*)converted_image_data.pixels.data());

    std::vector<float> colors{ 0.0f, 0.0f, 0.0f, 1.0f };
    for (uint32_t y = 0u; y < converted_image_data.height; y++)
    {
        for (uint32_t x = 0u; x < converted_image_data.width; x++)
        {
            for (uint32_t c = 0u; c < converted_image_data.channels; c++)
            {
                if (c < image_data.channels)
                {
                    colors[c] = image_buf.getchannel(x, y, 0, c);
                }
            }

            converted_image_buf.setpixel(x, y, 0, OIIO::cspan<float>(colors));
        }
    }

    return converted_image_data;
}

std::optional<ImageData> convertImageDataColorSpace(ColorPrimaries primaries, TransferFunction transfer, ImageState image_state, const ImageData& image_data)
{
    OIIO::TypeDesc format = getChannelFormat(image_data.channel_format);
    if (format == OIIO::TypeDesc::UNKNOWN)
    {
        return {};
    }
    uint32_t channel_size = getChannelFormatSize(image_data.channel_format);

    OIIO::ImageSpec image_spec{ (int)image_data.width, (int)image_data.height, (int)image_data.channels, format };
    OIIO::ImageBuf image_buf(image_spec, (void*)image_data.pixels.data());

    ImageData converted_image_data{};
    converted_image_data.width = image_data.width;
    converted_image_data.height = image_data.height;
    converted_image_data.channels = image_data.channels;
    converted_image_data.channel_format = image_data.channel_format;
    converted_image_data.primaries = primaries;
    converted_image_data.transfer = transfer;
    converted_image_data.image_state = image_state;
    converted_image_data.pixels.resize(converted_image_data.width * converted_image_data.height * converted_image_data.channels * channel_size);

    OIIO::ImageSpec converted_image_spec{ (int)converted_image_data.width, (int)converted_image_data.height, (int)converted_image_data.channels, format };
    OIIO::ImageBuf converted_image_buf(converted_image_spec, (void*)converted_image_data.pixels.data());

    //

    float3x3 to_xyz{};
    switch (image_data.primaries)
    {
        case ColorPrimaries::REC709:
            to_xyz = rgbToXYZ(COLOR_PRIMARY_REC709);
            break;
        case ColorPrimaries::REC2020:
            to_xyz = rgbToXYZ(COLOR_PRIMARY_REC2020);
            break;
        case ColorPrimaries::UNKNOWN:
        default:
            return {};
    }

    float3x3 from_xyz{};
    switch (converted_image_data.primaries)
    {
        case ColorPrimaries::REC709:
            from_xyz = inverse(rgbToXYZ(COLOR_PRIMARY_REC709));
            break;
        case ColorPrimaries::REC2020:
            from_xyz = inverse(rgbToXYZ(COLOR_PRIMARY_REC2020));
            break;
        case ColorPrimaries::UNKNOWN:
        default:
            return {};
    }

    const float3x3 conversion = from_xyz * to_xyz;

    std::vector<float> colors{ 0.0f, 0.0f, 0.0f, 1.0f };
    for (uint32_t y = 0u; y < converted_image_data.height; y++)
    {
        for (uint32_t x = 0u; x < converted_image_data.width; x++)
        {
            for (uint32_t c = 0u; c < converted_image_data.channels; c++)
            {
                if (c < image_data.channels)
                {
                    colors[c] = image_buf.getchannel(x, y, 0, c);
                }
            }

            float3 output_colors{ colors[0], colors[1], colors[2] };

            // Non-linear to linear if required
            if (image_data.transfer != TransferFunction::LINEAR)
            {
                switch (image_data.transfer)
                {
                    case TransferFunction::SRGB:
                        output_colors = srgbToLinear709(output_colors);
                        break;
                    case TransferFunction::EXTENDED_SRGB:
                        output_colors = scrgbToLinear709(output_colors);
                        break;
                    case TransferFunction::BT709:
                        output_colors = bt709ToLinear709(output_colors);
                        break;
                    case TransferFunction::BT2020:
                        output_colors = bt2020ToLinear2020(output_colors);
                        break;
                    case TransferFunction::ST2084_PQ:
                        output_colors = pqToLinear2020(output_colors);
                        break;
                    case TransferFunction::HLG:
                        output_colors = hlgToLinear2020(output_colors);
                        break;
                    case TransferFunction::LINEAR:
                    case TransferFunction::UNKNOWN:
                    default:
                        return {};
                }
            }

            // Source primaries -> XYZ -> target primaries
            output_colors = conversion * output_colors;

            // Linear to non-linear if required
            if (converted_image_data.transfer != TransferFunction::LINEAR)
            {
                switch (converted_image_data.transfer)
                {
                    case TransferFunction::SRGB:
                        output_colors = linear709ToSrgb(output_colors);
                        break;
                    case TransferFunction::EXTENDED_SRGB:
                        output_colors = linear709ToScrgb(output_colors);
                        break;
                    case TransferFunction::BT709:
                        output_colors = linear709ToBt709(output_colors);
                        break;
                    case TransferFunction::BT2020:
                        output_colors = linear2020ToBt2020(output_colors);
                        break;
                    case TransferFunction::ST2084_PQ:
                        output_colors = linear2020ToPq(output_colors);
                        break;
                    case TransferFunction::HLG:
                        output_colors = linear2020ToHlg(output_colors);
                        break;
                    case TransferFunction::LINEAR:
                    case TransferFunction::UNKNOWN:
                    default:
                        return {};
                }
            }

            colors[0] = output_colors[0];
            colors[1] = output_colors[1];
            colors[2] = output_colors[2];

            converted_image_buf.setpixel(x, y, 0, OIIO::cspan<float>(colors));
        }
    }

    return converted_image_data;
}

std::vector<ImageData> generateMipMaps(const ImageData& image_data)
{
    std::vector<ImageData> mip_levels;
    mip_levels.push_back(image_data);

    OIIO::TypeDesc format = getChannelFormat(image_data.channel_format);
    if (format == OIIO::TypeDesc::UNKNOWN)
    {
        return mip_levels;
    }

    uint32_t channel_size = getChannelFormatSize(image_data.channel_format);

    // Always resize from level 0 to avoid accumulated resampling error
    OIIO::ImageSpec base_spec{ (int)image_data.width, (int)image_data.height, (int)image_data.channels, format };
    OIIO::ImageBuf base_buf(base_spec, (void*)image_data.pixels.data());

    uint32_t width = image_data.width;
    uint32_t height = image_data.height;

    while (width > 1u || height > 1u)
    {
        uint32_t mip_width = std::max(width / 2u, 1u);
        uint32_t mip_height = std::max(height / 2u, 1u);

        OIIO::ROI roi{ 0, (int)mip_width, 0, (int)mip_height };
        OIIO::ImageBuf dst_buf = OIIO::ImageBufAlgo::resize(base_buf, "box", 0.0f, roi);

        ImageData mip_data{};
        mip_data.width = mip_width;
        mip_data.height = mip_height;
        mip_data.channels = image_data.channels;
        mip_data.channel_format = image_data.channel_format;
        mip_data.primaries = image_data.primaries;
        mip_data.transfer = image_data.transfer;
        mip_data.image_state = image_data.image_state;
        mip_data.pixels.resize(mip_width * mip_height * image_data.channels * channel_size);

        dst_buf.get_pixels(OIIO::ROI::All(), format, mip_data.pixels.data());

        mip_levels.push_back(std::move(mip_data));

        width = mip_width;
        height = mip_height;
    }

    return mip_levels;
}
