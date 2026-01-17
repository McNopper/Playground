#include "matrix_transform.h"

#include <cmath>

#include "helper.h"

float4x4 translationMatrix(const float3& t)
{
	return float4x4{
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ t.x, t.y, t.z, 1.0f }
	};
}

float4x4 scaleMatrix(const float3& s)
{
	return float4x4{
		{ s.x, 0.0f, 0.0f, 0.0f },
		{ 0.0f, s.y, 0.0f, 0.0f },
		{ 0.0f, 0.0f, s.z, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};
}

float4x4 rotateRxMatrix(float degree)
{
	float r = degreeToRadians(degree);

	float s = std::sin(r);
	float c = std::cos(r);

	return float4x4{
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, c, s, 0.0f },
		{ 0.0f, -s, c, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};
}

float4x4 rotateRyMatrix(float degree)
{
	float r = degreeToRadians(degree);

	float s = std::sin(r);
	float c = std::cos(r);

	return float4x4{
		{ c, 0.0f, -s, 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ s, 0.0f, c, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};
}

float4x4 rotateRzMatrix(float degree)
{
	float r = degreeToRadians(degree);

	float s = std::sin(r);
	float c = std::cos(r);

	return float4x4{
		{ c, s, 0.0f, 0.0f },
		{ -s, c, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};
}
