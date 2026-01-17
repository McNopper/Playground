#include "interpolate.h"

#include <cmath>

#include "vector.h"
#include "quaternion.h"

float min(float x, float y)
{
	if (x < y)
	{
		return x;
	}

	return y;
}

float2 min(const float2& x, const float2& y)
{
	return {
		min(x[0], y[0]),
		min(x[1], y[1])
	};
}

float3 min(const float3& x, const float3& y)
{
	return {
		min(x[0], y[0]),
		min(x[1], y[1]),
		min(x[2], y[2])
	};
}

float4 min(const float4& x, const float4& y)
{
	return {
		min(x[0], y[0]),
		min(x[1], y[1]),
		min(x[2], y[2]),
		min(x[3], y[3])
	};
}

float max(float x, float y)
{
	if (x > y)
	{
		return x;
	}

	return y;
}

float2 max(const float2& x, const float2& y)
{
	return {
		max(x[0], y[0]),
		max(x[1], y[1])
	};
}

float3 max(const float3& x, const float3& y)
{
	return {
		max(x[0], y[0]),
		max(x[1], y[1]),
		max(x[2], y[2])
	};
}

float4 max(const float4& x, const float4& y)
{
	return {
		max(x[0], y[0]),
		max(x[1], y[1]),
		max(x[2], y[2]),
		max(x[3], y[3])
	};
}

float clamp(float x, float min_val, float max_val)
{
	return max(min_val, min(x, max_val));
}

float2 clamp(const float2& x, const float2& min_val, const float2& max_val)
{
	return {
		clamp(x[0], min_val[0], max_val[0]),
		clamp(x[1], min_val[1], max_val[1])
	};
}

float3 clamp(const float3& x, const float3& min_val, const float3& max_val)
{
	return {
		clamp(x[0], min_val[0], max_val[0]),
		clamp(x[1], min_val[1], max_val[1]),
		clamp(x[2], min_val[2], max_val[2])
	};
}

float4 clamp(const float4& x, const float4& min_val, const float4& max_val)
{
	return {
		clamp(x[0], min_val[0], max_val[0]),
		clamp(x[1], min_val[1], max_val[1]),
		clamp(x[2], min_val[2], max_val[2]),
		clamp(x[3], min_val[3], max_val[3])
	};
}

float lerp(float x, float y, float t)
{
	return x * (1.0f - t) + y * t;
}

float2 lerp(const float2& x, const float2& y, float t)
{
	return x * (1.0f - t) + y * t;
}

float3 lerp(const float3& x, const float3& y, float t)
{
	return x * (1.0f - t) + y * t;
}

float4 lerp(const float4& x, const float4& y, float t)
{
	return x * (1.0f - t) + y * t;
}

float step(float edge, const float x)
{
	if (x < edge)
	{
		return 0.0f;
	}

	return 1.0f;
}

float2 step(const float2& edge, const float2& x)
{
	return {
		step(edge[0], x[0]),
		step(edge[1], x[1])
	};
}

float3 step(const float3& edge, const float3& x)
{
	return {
		step(edge[0], x[0]),
		step(edge[1], x[1]),
		step(edge[2], x[2])
	};
}

float4 step(const float4& edge, const float4& x)
{
	return {
		step(edge[0], x[0]),
		step(edge[1], x[1]),
		step(edge[2], x[2]),
		step(edge[3], x[3])
	};
}

float2 step(float edge, const float2& x)
{
	return {
		step(edge, x[0]),
		step(edge, x[1])
	};
}

float3 step(float edge, const float3& x)
{
	return {
		step(edge, x[0]),
		step(edge, x[1]),
		step(edge, x[2])
	};
}

float4 step(float edge, const float4& x)
{
	return {
		step(edge, x[0]),
		step(edge, x[1]),
		step(edge, x[2]),
		step(edge, x[3])
	};
}

float smoothstep(float edge0, float edge1, float x)
{
	float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);

	return t * t * (3.0f - (2.0f * t));
}

float2 smoothstep(const float2& edge0, const float2& edge1, const float2& x)
{
	return {
		smoothstep(edge0[0], edge1[0], x[0]),
		smoothstep(edge0[1], edge1[1], x[1])
	};
}

float3 smoothstep(const float3& edge0, const float3& edge1, const float3& x)
{
	return {
		smoothstep(edge0[0], edge1[0], x[0]),
		smoothstep(edge0[1], edge1[1], x[1]),
		smoothstep(edge0[2], edge1[2], x[2])
	};
}

float4 smoothstep(const float4& edge0, const float4& edge1, const float4& x)
{
	return {
		smoothstep(edge0[0], edge1[0], x[0]),
		smoothstep(edge0[1], edge1[1], x[1]),
		smoothstep(edge0[2], edge1[2], x[2]),
		smoothstep(edge0[3], edge1[3], x[3])
	};
}

float2 smoothstep(float edge0, float edge1, const float2& x)
{
	return {
		smoothstep(edge0, edge1, x[0]),
		smoothstep(edge0, edge1, x[1])
	};
}

float3 smoothstep(float edge0, float edge1, const float3& x)
{
	return {
		smoothstep(edge0, edge1, x[0]),
		smoothstep(edge0, edge1, x[1]),
		smoothstep(edge0, edge1, x[2])
	};
}

float4 smoothstep(float edge0, float edge1, const float4& x)
{
	return {
		smoothstep(edge0, edge1, x[0]),
		smoothstep(edge0, edge1, x[1]),
		smoothstep(edge0, edge1, x[2]),
		smoothstep(edge0, edge1, x[3])
	};
}

// see https://en.wikipedia.org/wiki/Slerp
quaternion slerp(const quaternion& x, const quaternion& y, float t)
{
	float omega = std::acos(x[0] * y[0] + x[1] * y[1] + x[2] * y[2] + x[3] * y[3]);

	float so = std::sin(omega);

	return x * ((1.0f - t) * omega) / so + y * (t * omega) / so;
}
