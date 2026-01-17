#include "image_data.h"

#include <algorithm>
#include <cstring>

#include <OpenImageIO/imageio.h>
#include <OpenImageIO/imagebuf.h>
#include <OpenImageIO/span.h>

#include "core/math/matrix.h"
#include "core/color/convert.h"

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

	if (color_space == "linear" || color_space == "lin_srgb")
	{
		image_data.color_space = ColorSpace::SRGB;
		image_data.linear = true;
	}
	else if (color_space == "lin_rec709")
	{
		image_data.color_space = ColorSpace::BT709;
		image_data.linear = true;
	}
	else if (color_space == "srgb")
	{
		image_data.color_space = ColorSpace::SRGB;
		image_data.linear = false;
	}
	else if (color_space == "rec709")
	{
		image_data.color_space = ColorSpace::BT709;
		image_data.linear = false;
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
	converted_image_data.color_space = image_data.color_space;
	converted_image_data.linear = image_data.linear;
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

std::optional<ImageData> convertImageDataColorSpace(ColorSpace color_space, bool linear, const ImageData& image_data)
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
	converted_image_data.color_space = color_space;
	converted_image_data.linear = linear;
	converted_image_data.pixels.resize(converted_image_data.width * converted_image_data.height * converted_image_data.channels * channel_size);

	OIIO::ImageSpec converted_image_spec{ (int)converted_image_data.width, (int)converted_image_data.height, (int)converted_image_data.channels, format };
	OIIO::ImageBuf converted_image_buf(converted_image_spec, (void*)converted_image_data.pixels.data());

	//

	float3x3 to_xyz{};
	switch (image_data.color_space)
	{
	case ColorSpace::SRGB:
		to_xyz = toXYZ(COLOR_PRIMARY_SRGB);
		break;
	case ColorSpace::SCRGB:
		to_xyz = toXYZ(COLOR_PRIMARY_SCRGB);
		break;
	case ColorSpace::BT709:
		to_xyz = toXYZ(COLOR_PRIMARY_BT709);
		break;
	case ColorSpace::BT2020:
		to_xyz = toXYZ(COLOR_PRIMARY_BT2020);
		break;
	case ColorSpace::UNKNOWN:
	default:
		return {};
	}

	float3x3 from_xyz{};
	switch (converted_image_data.color_space)
	{
	case ColorSpace::SRGB:
		from_xyz = toXYZ(COLOR_PRIMARY_SRGB);
		break;
	case ColorSpace::SCRGB:
		from_xyz = toXYZ(COLOR_PRIMARY_SCRGB);
		break;
	case ColorSpace::BT709:
		from_xyz = toXYZ(COLOR_PRIMARY_BT709);
		break;
	case ColorSpace::BT2020:
		from_xyz = toXYZ(COLOR_PRIMARY_BT2020);
		break;
	case ColorSpace::UNKNOWN:
	default:
		return {};
	}
	from_xyz = inverse(from_xyz);

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
			if (!image_data.linear)
			{
				switch (image_data.color_space)
				{
				case ColorSpace::SRGB:
					output_colors = toLinearSRGB(output_colors);
					break;
				case ColorSpace::SCRGB:
					output_colors = toLinearSCRGB(output_colors);
					break;
				case ColorSpace::BT709:
					output_colors = toLinearBT706(output_colors);
					break;
				case ColorSpace::BT2020:
					output_colors = toLinearBT2020(output_colors);
					break;
				case ColorSpace::UNKNOWN:
				default:
					return {};
				}
			}

			// Old color space -> XYZ color space -> new color space
			output_colors = from_xyz * to_xyz * output_colors;

			// Linear to non-linear if required
			if (!converted_image_data.linear)
			{
				switch (converted_image_data.color_space)
				{
				case ColorSpace::SRGB:
					output_colors = toNonLinearSRGB(output_colors);
					break;
				case ColorSpace::SCRGB:
					output_colors = toNonLinearSCRGB(output_colors);
					break;
				case ColorSpace::BT709:
					output_colors = toNonLinearBT706(output_colors);
					break;
				case ColorSpace::BT2020:
					output_colors = toNonLinearBT2020(output_colors);
					break;
				case ColorSpace::UNKNOWN:
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
