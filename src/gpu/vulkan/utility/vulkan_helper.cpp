#include "vulkan_helper.h"

#include "core/image/image_data.h"

VkFormat getVulkanFormat(const ImageData& image_data)
{
    if (image_data.channel_format == ChannelFormat::UNORM)
    {
        bool use_srgb = !image_data.linear && image_data.color_space == ColorSpace::SRGB;
        switch (image_data.channels)
        {
        case 1: return use_srgb ? VK_FORMAT_R8_SRGB : VK_FORMAT_R8_UNORM;
        case 2: return use_srgb ? VK_FORMAT_R8G8_SRGB : VK_FORMAT_R8G8_UNORM;
        case 3: return use_srgb ? VK_FORMAT_R8G8B8_SRGB : VK_FORMAT_R8G8B8_UNORM;
        case 4: return use_srgb ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM;
        default: break;
        }
    }
    else if (image_data.channel_format == ChannelFormat::SHALF)
    {
        switch (image_data.channels)
        {
        case 1: return VK_FORMAT_R16_SFLOAT;
        case 2: return VK_FORMAT_R16G16_SFLOAT;
        case 3: return VK_FORMAT_R16G16B16_SFLOAT;
        case 4: return VK_FORMAT_R16G16B16A16_SFLOAT;
        default: break;
        }
    }
    else if (image_data.channel_format == ChannelFormat::SFLOAT)
    {
        switch (image_data.channels)
        {
        case 1: return VK_FORMAT_R32_SFLOAT;
        case 2: return VK_FORMAT_R32G32_SFLOAT;
        case 3: return VK_FORMAT_R32G32B32_SFLOAT;
        case 4: return VK_FORMAT_R32G32B32A32_SFLOAT;
        default: break;
        }
    }

    return VK_FORMAT_UNDEFINED;
}

uint32_t getFormatSize(VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_UNDEFINED:
            return 0u;
        case VK_FORMAT_R16_UINT:
        case VK_FORMAT_R16_SINT:
        case VK_FORMAT_R16_SFLOAT:
            return 2u;
        case VK_FORMAT_R16G16_UINT:
        case VK_FORMAT_R16G16_SINT:
        case VK_FORMAT_R16G16_SFLOAT:
            return 4u;
        case VK_FORMAT_R16G16B16_UINT:
        case VK_FORMAT_R16G16B16_SINT:
        case VK_FORMAT_R16G16B16_SFLOAT:
            return 6u;
        case VK_FORMAT_R16G16B16A16_UINT:
        case VK_FORMAT_R16G16B16A16_SINT:
        case VK_FORMAT_R16G16B16A16_SFLOAT:
            return 8u;
        case VK_FORMAT_R32_UINT:
        case VK_FORMAT_R32_SINT:
        case VK_FORMAT_R32_SFLOAT:
            return 4u;
        case VK_FORMAT_R32G32_UINT:
        case VK_FORMAT_R32G32_SINT:
        case VK_FORMAT_R32G32_SFLOAT:
            return 8u;
        case VK_FORMAT_R32G32B32_UINT:
        case VK_FORMAT_R32G32B32_SINT:
        case VK_FORMAT_R32G32B32_SFLOAT:
            return 12u;
        case VK_FORMAT_R32G32B32A32_UINT:
        case VK_FORMAT_R32G32B32A32_SINT:
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return 16u;
        case VK_FORMAT_R64_UINT:
        case VK_FORMAT_R64_SINT:
        case VK_FORMAT_R64_SFLOAT:
            return 8u;
        case VK_FORMAT_R64G64_UINT:
        case VK_FORMAT_R64G64_SINT:
        case VK_FORMAT_R64G64_SFLOAT:
            return 16u;
        case VK_FORMAT_R64G64B64_UINT:
        case VK_FORMAT_R64G64B64_SINT:
        case VK_FORMAT_R64G64B64_SFLOAT:
            return 24u;
        case VK_FORMAT_R64G64B64A64_UINT:
        case VK_FORMAT_R64G64B64A64_SINT:
        case VK_FORMAT_R64G64B64A64_SFLOAT:
            return 32u;

        default:
            break;
    }

    // Not implemented.
    return 0u;
}

bool isFormatDepthOnly(VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_D16_UNORM:
        case VK_FORMAT_X8_D24_UNORM_PACK32:
        case VK_FORMAT_D32_SFLOAT:
            return true;

        default:
            break;
    }

    return false;
}

bool isFormatStencilOnly(VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_S8_UINT:
            return true;

        default:
            break;
    }

    return false;
}

bool isFormatDepthStencilOnly(VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return true;

        default:
            break;
    }

    return false;
}

bool isFormatDepthStencil(VkFormat format)
{
    return isFormatDepthStencilOnly(format) || isFormatDepthOnly(format) || isFormatStencilOnly(format);
}

bool isFormatSRGB(VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_R8_SRGB:
        case VK_FORMAT_R8G8_SRGB:
        case VK_FORMAT_R8G8B8_SRGB:
        case VK_FORMAT_B8G8R8_SRGB:
        case VK_FORMAT_R8G8B8A8_SRGB:
        case VK_FORMAT_B8G8R8A8_SRGB:
        case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
        case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
        case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
        case VK_FORMAT_BC2_SRGB_BLOCK:
        case VK_FORMAT_BC3_SRGB_BLOCK:
        case VK_FORMAT_BC7_SRGB_BLOCK:
        case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
        case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
        case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
        case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
        case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
        case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
        case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
        case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
        case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
        case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
        case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
        case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
        case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
        case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
        case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
        case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
        case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:

        case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG:
        case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG:
        case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG:
        case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:
        return true;

        default:
            break;
    }

    return false;
}

bool isColorSpaceLinear(VkColorSpaceKHR color_space)
{
    switch (color_space)
    {
        case VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT:
        case VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT:
        case VK_COLOR_SPACE_BT709_LINEAR_EXT:
        case VK_COLOR_SPACE_BT2020_LINEAR_EXT:
        case VK_COLOR_SPACE_HDR10_ST2084_EXT:
        case VK_COLOR_SPACE_HDR10_HLG_EXT:
        case VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT:
        case VK_COLOR_SPACE_PASS_THROUGH_EXT:
            return true;

        default:
            break;
    }

    // Rest is not implemented and/or not linear.
    return false;
}

ColorSpace getColorSpace(VkColorSpaceKHR color_space)
{
    switch (color_space)
    {
        case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:
            return ColorSpace::SRGB;

        case VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT:
        case VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT:
            return ColorSpace::SCRGB;

        case VK_COLOR_SPACE_BT709_LINEAR_EXT:
        case VK_COLOR_SPACE_BT709_NONLINEAR_EXT:
            return ColorSpace::BT709;

        case VK_COLOR_SPACE_BT2020_LINEAR_EXT:
        case VK_COLOR_SPACE_HDR10_ST2084_EXT:
        case VK_COLOR_SPACE_HDR10_HLG_EXT:
            return ColorSpace::BT2020;

        default:
            break;
    }

    // Rest is not implemented.
    return ColorSpace::UNKNOWN;
}

TransferFunction getTransferFunction(VkColorSpaceKHR color_space)
{
    switch (color_space)
    {
        case VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT:
        case VK_COLOR_SPACE_BT709_LINEAR_EXT:
        case VK_COLOR_SPACE_BT2020_LINEAR_EXT:
            return TransferFunction::LINEAR;

        case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:
            return TransferFunction::SRGB;

        case VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT:
            return TransferFunction::SCRGB;

        case VK_COLOR_SPACE_BT709_NONLINEAR_EXT:
            return TransferFunction::BT709;

        case VK_COLOR_SPACE_HDR10_ST2084_EXT:
            return TransferFunction::ST2084_PQ;

        case VK_COLOR_SPACE_HDR10_HLG_EXT:
            return TransferFunction::HLG;

        default:
            break;
    }

    // Rest is not implemented.
    return TransferFunction::UNKNOWN;
}

TransferFunction getTransferFunction(VkSurfaceFormatKHR surface_format)
{
    auto transfer_function = getTransferFunction(surface_format.colorSpace);

    // In this case, no transfer function is required as the surface is handling the conversion.
    if (isFormatSRGB(surface_format.format) && transfer_function == TransferFunction::SRGB)
    {
        transfer_function = TransferFunction::LINEAR;
    }

    return transfer_function;
}
