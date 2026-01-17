#ifndef CORE_COLOR_CONVERT_H_
#define CORE_COLOR_CONVERT_H_

#include "types.h"

float3x3 toXYZ(const ChromaticityCoordinates& c);

float3 toLinearSRGB(const float3& color);
float3 toNonLinearSRGB(const float3& color);

float3 toLinearSCRGB(const float3& color);
float3 toNonLinearSCRGB(const float3& color);

float3 toLinearBT706(const float3& color);
float3 toNonLinearBT706(const float3& color);

float3 toLinearBT2020(const float3& color);
float3 toNonLinearBT2020(const float3& color);

float3 toLinearPQ(const float3& color);
float3 toNonLinearPQ(const float3& color);

float3 toLinearHLG(const float3& color);
float3 toNonLinearHLG(const float3& color);

#endif /* CORE_COLOR_CONVERT_H_ */
