#ifndef CORE_COLOR_CONVERT_H_
#define CORE_COLOR_CONVERT_H_

#include "types.h"

float3x3 rgbToXYZ(const ChromaticityCoordinates& c);

float3 srgbToLinear709(const float3& color);
float3 linear709ToSrgb(const float3& color);

float3 scrgbToLinear709(const float3& color);
float3 linear709ToScrgb(const float3& color);

float3 gamma18ToLinear709(const float3& color);
float3 linear709ToGamma18(const float3& color);

float3 gamma22ToLinear709(const float3& color);
float3 linear709ToGamma22(const float3& color);

float3 gamma24ToLinear709(const float3& color);
float3 linear709ToGamma24(const float3& color);

float3 bt709ToLinear709(const float3& color);
float3 linear709ToBt709(const float3& color);

float3 bt2020ToLinear2020(const float3& color);
float3 linear2020ToBt2020(const float3& color);

float3 pqToLinear2020(const float3& color);
float3 linear2020ToPq(const float3& color);

float3 hlgToLinear2020(const float3& color);
float3 linear2020ToHlg(const float3& color);

#endif /* CORE_COLOR_CONVERT_H_ */
