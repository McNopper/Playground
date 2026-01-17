#ifndef CORE_MATH_INTERPOLATE_H_
#define CORE_MATH_INTERPOLATE_H_

#include "vector.h"

#include "quaternion.h"

float min(float x, float y);
float2 min(const float2& x, const float2& y);
float3 min(const float3& x, const float3& y);
float4 min(const float4& x, const float4& y);

float max(float x, float y);
float2 max(const float2& x, const float2& y);
float3 max(const float3& x, const float3& y);
float4 max(const float4& x, const float4& y);

float clamp(float x, float min_val, float max_val);
float2 clamp(const float2& x, const float2& min_val, const float2& max_val);
float3 clamp(const float3& x, const float3& min_val, const float3& max_val);
float4 clamp(const float4& x, const float4& min_val, const float4& max_val);

// also known as mix
float lerp(float x, float y, float t);
float2 lerp(const float2& x, const float2& y, float t);
float3 lerp(const float3& x, const float3& y, float t);
float4 lerp(const float4& x, const float4& y, float t);

float step(float edge, const float x);
float2 step(const float2& edge, const float2& x);
float3 step(const float3& edge, const float3& x);
float4 step(const float4& edge, const float4& x);
float2 step(float edge, const float2& x);
float3 step(float edge, const float3& x);
float4 step(float edge, const float4& x);

float smoothstep(float edge0, const float edge1, const float x);
float2 smoothstep(const float2& edge0, const float2& edge1, const float2& x);
float3 smoothstep(const float3& edge0, const float3& edge1, const float3& x);
float4 smoothstep(const float4& edge0, const float4& edge1, const float4& x);
float2 smoothstep(float edge0, float edge1, const float2& x);
float3 smoothstep(float edge0, float edge1, const float3& x);
float4 smoothstep(float edge0, float edge1, const float4& x);

quaternion slerp(const quaternion& x, const quaternion& y, float t);

#endif /* CORE_MATH_INTERPOLATE_H_ */
