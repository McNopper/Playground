#include "matrix.h"

#include <cmath>

const float2& float2x2::operator[](std::uint32_t index) const
{
	return columns[index];
}

float2& float2x2::operator[](std::uint32_t index)
{
	return columns[index];
}

float2x2::float2x2(float scalar)
{
	columns[0][0] = scalar;
	columns[1][1] = scalar;
}

float2x2::float2x2(const float2& col0, const float2& col1)
{
	columns[0] = col0;
	columns[1] = col1;
}

float2x2::float2x2(const float2x2& other)
{
	columns[0] = other.columns[0];
	columns[1] = other.columns[1];
}

float2x2::float2x2(const float3x3& other)
{
	columns[0] = { other.columns[0][0], other.columns[0][1] };
	columns[1] = { other.columns[1][0], other.columns[1][1] };
}

float2x2::float2x2(const float4x4& other)
{
	columns[0] = { other.columns[0][0], other.columns[0][1] };
	columns[1] = { other.columns[1][0], other.columns[1][1] };
}

const float3& float3x3::operator[](std::uint32_t index) const
{
	return columns[index];
}

float3& float3x3::operator[](std::uint32_t index)
{
	return columns[index];
}

float3x3::float3x3(float scalar)
{
	columns[0][0] = scalar;
	columns[1][1] = scalar;
	columns[2][2] = scalar;
}

float3x3::float3x3(const float3& col0, const float3& col1, const float3& col2)
{
	columns[0] = col0;
	columns[1] = col1;
	columns[2] = col2;
}

float3x3::float3x3(const float2x2& other)
{
	columns[0] = { other.columns[0], 0.0f };
	columns[1] = { other.columns[1], 0.0f };
	columns[2] = { 0.0f, 0.0f, 1.0f };
}

float3x3::float3x3(const float3x3& other)
{
	columns[0] = other.columns[0];
	columns[1] = other.columns[1];
	columns[2] = other.columns[2];
}

float3x3::float3x3(const float4x4& other)
{
	columns[0] = { other.columns[0][0], other.columns[0][1], other.columns[0][2] };
	columns[1] = { other.columns[1][0], other.columns[1][1], other.columns[1][2] };
	columns[2] = { other.columns[2][0], other.columns[2][1], other.columns[2][2] };
}

const float4& float4x4::operator[](std::uint32_t index) const
{
	return columns[index];
}

float4& float4x4::operator[](std::uint32_t index)
{
	return columns[index];
}

float4x4::float4x4(float scalar)
{
	columns[0][0] = scalar;
	columns[1][1] = scalar;
	columns[2][2] = scalar;
	columns[3][3] = scalar;
}

float4x4::float4x4(const float4& col0, const float4& col1, const float4& col2, const float4& col3)
{
	columns[0] = col0;
	columns[1] = col1;
	columns[2] = col2;
	columns[3] = col3;
}

float4x4::float4x4(const float2x2& other)
{
	columns[0] = { other.columns[0], 0.0f, 0.0f };
	columns[1] = { other.columns[1], 0.0f, 0.0f };
	columns[2] = { 0.0f, 0.0f, 1.0f, 0.0f };
	columns[3] = { 0.0f, 0.0f, 0.0f, 1.0f };
}

float4x4::float4x4(const float3x3& other)
{
	columns[0] = { other.columns[0], 0.0f };
	columns[1] = { other.columns[1], 0.0f };
	columns[2] = { other.columns[2], 0.0f };
	columns[3] = { 0.0f, 0.0f, 0.0f, 1.0f };
}

float4x4::float4x4(const float4x4& other)
{
	columns[0] = other.columns[0];
	columns[1] = other.columns[1];
	columns[2] = other.columns[2];
	columns[3] = other.columns[3];
}

float2x2 operator*(float s, const float2x2& x)
{
	return float2x2{
		{s * x[0][0], s * x[0][1]},
		{s * x[1][0], s * x[1][1]}
	};
}

float2x2 operator*(const float2x2& x, float s)
{
	return float2x2{
		{x[0][0] * s, x[0][1] * s},
		{x[1][0] * s, x[1][1] * s}
	};
}

float3x3 operator*(float s, const float3x3& x)
{
	return float3x3{
		{s * x[0][0], s * x[0][1], s * x[0][2]},
		{s * x[1][0], s * x[1][1], s * x[1][2]},
		{s * x[2][0], s * x[2][1], s * x[2][2]}
	};
}

float3x3 operator*(const float3x3& x, float s)
{
	return float3x3{
		{x[0][0] * s, x[0][1] * s, x[0][2] * s},
		{x[1][0] * s, x[1][1] * s, x[1][2] * s},
		{x[2][0] * s, x[2][1] * s, x[2][2] * s}
	};
}

float4x4 operator*(float s, const float4x4& x)
{
	return float4x4{
		{s * x[0][0], s * x[0][1], s * x[0][2], s * x[0][3]},
		{s * x[1][0], s * x[1][1], s * x[1][2], s * x[1][3]},
		{s * x[2][0], s * x[2][1], s * x[2][2], s * x[2][3]},
		{s * x[3][0], s * x[3][1], s * x[3][2], s * x[3][3]}
	};
}

float4x4 operator*(const float4x4& x, float s)
{
	return float4x4{
		{x[0][0] * s, x[0][1] * s, x[0][2] * s, x[0][3] * s},
		{x[1][0] * s, x[1][1] * s, x[1][2] * s, x[1][3] * s},
		{x[2][0] * s, x[2][1] * s, x[2][2] * s, x[2][3] * s},
		{x[3][0] * s, x[3][1] * s, x[3][2] * s, x[3][3] * s}
	};
}

float2 operator*(const float2& v, const float2x2& x)
{
	return float2{
		x[0][0] * v[0] + x[0][1] * v[1],
		x[1][0] * v[0] + x[1][1] * v[1]
	};
}

float2 operator*(const float2x2& x, const float2& v)
{
	return float2{
		x[0][0] * v[0] + x[1][0] * v[1],
		x[0][1] * v[0] + x[1][1] * v[1]
	};
}

float3 operator*(const float3& v, const float3x3& x)
{
	return float3{
		x[0][0] * v[0] + x[0][1] * v[1] + x[0][2] * v[2],
		x[1][0] * v[0] + x[1][1] * v[1] + x[1][2] * v[2],
		x[2][0] * v[0] + x[2][1] * v[1] + x[2][2] * v[2]
	};
}

float3 operator*(const float3x3& x, const float3& v)
{
	return float3{
		x[0][0] * v[0] + x[1][0] * v[1] + x[2][0] * v[2],
		x[0][1] * v[0] + x[1][1] * v[1] + x[2][1] * v[2],
		x[0][2] * v[0] + x[1][2] * v[1] + x[2][2] * v[2]
	};
}

float4 operator*(const float4& v, const float4x4& x)
{
	return float4{
		x[0][0] * v[0] + x[0][1] * v[1] + x[0][2] * v[2] + x[0][3] * v[3],
		x[1][0] * v[0] + x[1][1] * v[1] + x[1][2] * v[2] + x[1][3] * v[3],
		x[2][0] * v[0] + x[2][1] * v[1] + x[2][2] * v[2] + x[2][3] * v[3],
		x[3][0] * v[0] + x[3][1] * v[1] + x[3][2] * v[2] + x[3][3] * v[3]
	};
}

float4 operator*(const float4x4& x, const float4& v)
{
	return float4{
		x[0][0] * v[0] + x[1][0] * v[1] + x[2][0] * v[2] + x[3][0] * v[3],
		x[0][1] * v[0] + x[1][1] * v[1] + x[2][1] * v[2] + x[3][1] * v[3],
		x[0][2] * v[0] + x[1][2] * v[1] + x[2][2] * v[2] + x[3][2] * v[3],
		x[0][3] * v[0] + x[1][3] * v[1] + x[2][3] * v[2] + x[3][3] * v[3]
	};
}

float2x2 operator*(const float2x2& x, const float2x2& y)
{
	float2x2 result{};

	for (std::uint32_t c = 0u; c < 2u; c++)
	{
		for (std::uint32_t r = 0u; r < 2u; r++)
		{
			result.columns[c][r] = 0.0f;
			for (std::uint32_t k = 0u; k < 2u; k++)
			{
				result.columns[c][r] += x.columns[k][r] * y.columns[c][k];
			}
		}
	}

	return result;
}

float3x3 operator*(const float3x3& x, const float3x3& y)
{
	float3x3 result{};

	for (std::uint32_t c = 0u; c < 3u; c++)
	{
		for (std::uint32_t r = 0u; r < 3u; r++)
		{
			result.columns[c][r] = 0.0f;
			for (std::uint32_t k = 0u; k < 3u; k++)
			{
				result.columns[c][r] += x.columns[k][r] * y.columns[c][k];
			}
		}
	}

	return result;
}

float4x4 operator*(const float4x4& x, const float4x4& y)
{
	float4x4 result{};

	for (std::uint32_t c = 0u; c < 4u; c++)
	{
		for (std::uint32_t r = 0u; r < 4u; r++)
		{
			result.columns[c][r] = 0.0f;
			for (std::uint32_t k = 0u; k < 4u; k++)
			{
				result.columns[c][r] += x.columns[k][r] * y.columns[c][k];
			}
		}
	}

	return result;
}

float2 mul(const float2& v, const float2x2& x)
{
	return v * x;
}

float2 mul(const float2x2& x, const float2& v)
{
	return x * v;
}

float3 mul(const float3& v, const float3x3& x)
{
	return v * x;
}

float3 mul(const float3x3& x, const float3& v)
{
	return x * v;
}

float4 mul(const float4& v, const float4x4& x)
{
	return v * x;
}

float4 mul(const float4x4& x, const float4& v)
{
	return x * v;
}

float2x2 submatrix(const float3x3& x, std::uint32_t col, std::uint32_t row)
{
	float2x2 result{ 0.0f };

	std::uint32_t subi{ 0u };
	for (std::uint32_t i = 0u; i < 3u; i++)
	{
		if (i == col)
		{
			continue;
		}
		std::uint32_t subj{ 0u };
		for (std::uint32_t j = 0u; j < 3u; j++)
		{
			if (j == row)
			{
				continue;
			}
			result[subi][subj] = x[i][j];
			subj++;
		}
		subi++;
	}

	return result;
}

float3x3 submatrix(const float4x4& x, std::uint32_t col, std::uint32_t row)
{
	float3x3 result{ 0.0f };

	std::uint32_t subi{ 0u };
	for (std::uint32_t i = 0u; i < 4u; i++)
	{
		if (i == col)
		{
			continue;
		}
		std::uint32_t subj{ 0u };
		for (std::uint32_t j = 0u; j < 4u; j++)
		{
			if (j == row)
			{
				continue;
			}
			result[subi][subj] = x[i][j];
			subj++;
		}
		subi++;
	}

	return result;
}

float determinant(const float2x2& x)
{
	return x[0][0] * x[1][1] - x[0][1] * x[1][0];
}

float determinant(const float3x3& x)
{
	return x[0][0] * (x[1][1] * x[2][2] - x[1][2] * x[2][1]) - x[0][1] * (x[1][0] * x[2][2] - x[1][2] * x[2][0]) + x[0][2] * (x[1][0] * x[2][1] - x[1][1] * x[2][0]);
}

float determinant(const float4x4& x)
{
	return x[0][0] * determinant(submatrix(x, 0, 0))
		- x[0][1] * determinant(submatrix(x, 0, 1))
		+ x[0][2] * determinant(submatrix(x, 0, 2))
		- x[0][3] * determinant(submatrix(x, 0, 3));
}

float minor(const float2x2& x, std::uint32_t col, std::uint32_t row)
{
	return x[(col + 1u) % 2u][(row + 1u) % 2u];
}

float minor(const float3x3& x, std::uint32_t col, std::uint32_t row)
{
	return determinant(submatrix(x, col, row));
}

float minor(const float4x4& x, std::uint32_t col, std::uint32_t row)
{
	return determinant(submatrix(x, col, row));
}

float2x2 transpose(const float2x2& x)
{
	return float2x2{
	  {x[0][0], x[1][0]},
	  {x[0][1], x[1][1]}
	};
}

float3x3 transpose(const float3x3& x)
{
	return float3x3{
	  {x[0][0], x[1][0], x[2][0]},
	  {x[0][1], x[1][1], x[2][1]},
	  {x[0][2], x[1][2], x[2][2]}
	};
}

float4x4 transpose(const float4x4& x)
{
	return float4x4{
	  {x[0][0], x[1][0], x[2][0], x[3][0]},
	  {x[0][1], x[1][1], x[2][1], x[3][1]},
	  {x[0][2], x[1][2], x[2][2], x[3][2]},
	  {x[0][3], x[1][3], x[2][3], x[3][3]}
	};
}

float2x2 cofactor(const float2x2& x)
{
	float2x2 result{ 0.0f };

	for (std::uint32_t i = 0u; i < 2u; i++)
	{
		for (std::uint32_t j = 0u; j < 2u; j++)
		{
			result[i][j] = std::pow(-1.0f, (float)(i + j)) * minor(x, i, j);
		}
	}

	return result;
}

float3x3 cofactor(const float3x3& x)
{
	float3x3 result{ 0.0f };

	for (std::uint32_t i = 0u; i < 3u; i++)
	{
		for (std::uint32_t j = 0u; j < 3u; j++)
		{
			result[i][j] = std::pow(-1.0f, (float)(i + j)) * minor(x, i, j);
		}
	}

	return result;
}

float4x4 cofactor(const float4x4& x)
{
	float4x4 result{ 0.0f };

	for (std::uint32_t i = 0u; i < 4u; i++)
	{
		for (std::uint32_t j = 0u; j < 4u; j++)
		{
			result[i][j] = std::pow(-1.0f, (float)(i + j)) * minor(x, i, j);
		}
	}

	return result;
}

float2x2 adjoint(const float2x2& x)
{
	return transpose(cofactor(x));
}

float3x3 adjoint(const float3x3& x)
{
	return transpose(cofactor(x));
}

float4x4 adjoint(const float4x4& x)
{
	return transpose(cofactor(x));
}

float2x2 inverse(const float2x2& x)
{
	auto inv_det = 1.0f / determinant(x);

	return inv_det * adjoint(x);
}

float3x3 inverse(const float3x3& x)
{
	auto inv_det = 1.0f / determinant(x);

	return inv_det * adjoint(x);
}

float4x4 inverse(const float4x4& x)
{
	auto inv_det = 1.0f / determinant(x);

	return inv_det * adjoint(x);
}
