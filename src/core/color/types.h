#ifndef CORE_COLOR_TYPES_H_
#define CORE_COLOR_TYPES_H_

#include "core/math/matrix.h"
#include "core/math/vector.h"

// https://registry.khronos.org/VulkanSC/specs/1.0-extensions/man/html/VkColorSpaceKHR.html
// https://registry.khronos.org/DataFormat/specs/1.3/dataformat.1.3.html
// https://en.wikipedia.org/wiki/Perceptual_quantizer
// https://en.wikipedia.org/wiki/Hybrid_log%E2%80%93gamma
// https://github.com/AcademySoftwareFoundation/ColorInterop

enum class ColorPrimaries
{
    UNKNOWN,
    REC709,     // sRGB / BT.709 / scRGB primaries, D65 white point
    REC2020     // BT.2020 primaries, D65 white point
};

enum class TransferFunction
{
    UNKNOWN,
    LINEAR,
    SRGB,       // IEC 61966-2-1 piecewise
    EXTENDED_SRGB, // scRGB: sRGB extended to negative values
    BT709,      // ITU-R BT.709 OETF (gamma ~1/0.45)
    BT2020,     // ITU-R BT.2020 OETF (gamma ~1/0.45)
    ST2084_PQ,  // SMPTE ST 2084 / ITU-R BT.2100 PQ
    HLG         // ITU-R BT.2100 Hybrid Log-Gamma
};

enum class ImageState
{
    UNKNOWN,
    SCENE,      // Scene-referred (_scene suffix in interop IDs)
    DISPLAY     // Display-referred (_display suffix in interop IDs)
};

struct ChromaticityCoordinates
{
    float2 R{};
    float2 G{};
    float2 B{};
    float2 W{};
};

const float2 D65{ 0.3127f, 0.3290f };

const ChromaticityCoordinates COLOR_PRIMARY_REC709{
    { 0.640f, 0.330f },
    { 0.300f, 0.600f },
    { 0.150f, 0.060f },
    D65
};

const ChromaticityCoordinates COLOR_PRIMARY_REC2020{
    { 0.708f, 0.292f },
    { 0.170f, 0.797f },
    { 0.131f, 0.046f },
    D65
};

#endif /* CORE_COLOR_TYPES_H_ */
