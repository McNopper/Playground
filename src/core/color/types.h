#ifndef CORE_COLOR_TYPES_H_
#define CORE_COLOR_TYPES_H_

#include "core/math/vector.h"

#include "core/math/matrix.h"

// https://registry.khronos.org/VulkanSC/specs/1.0-extensions/man/html/VkColorSpaceKHR.html
// https://registry.khronos.org/DataFormat/specs/1.3/dataformat.1.3.html
// https://en.wikipedia.org/wiki/Perceptual_quantizer
// https://en.wikipedia.org/wiki/Hybrid_log%E2%80%93gamma

enum class ColorSpace {
    UNKNOWN,
    SRGB,
    SCRGB,
    BT709,
    BT2020
};

enum class TransferFunction {
    UNKNOWN,
    LINEAR,
    SRGB,
    SCRGB,
    BT709,
    ST2084_PQ,
    HLG
};

struct ChromaticityCoordinates {
    float2 R{};
    float2 G{};
    float2 B{};
    float2 W{};
};

const float2 D65{ 0.3127f, 0.3290f };

const ChromaticityCoordinates COLOR_PRIMARY_SRGB{
    {0.640f, 0.330f},
    {0.300f, 0.600f},
    {0.150f, 0.060f},
    D65
};

const ChromaticityCoordinates COLOR_PRIMARY_SCRGB = COLOR_PRIMARY_SRGB;

const ChromaticityCoordinates COLOR_PRIMARY_BT709 = COLOR_PRIMARY_SRGB;

const ChromaticityCoordinates COLOR_PRIMARY_BT2020{
    {0.708f, 0.292f},
    {0.170f, 0.797f},
    {0.131f, 0.046f},
    D65
};

#endif /* CORE_COLOR_TYPES_H_ */
