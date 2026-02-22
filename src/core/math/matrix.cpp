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

const float5& float5x5::operator[](std::uint32_t index) const
{
    return columns[index];
}

float5& float5x5::operator[](std::uint32_t index)
{
    return columns[index];
}

float5x5::float5x5(float scalar)
{
    columns[0][0] = scalar;
    columns[1][1] = scalar;
    columns[2][2] = scalar;
    columns[3][3] = scalar;
    columns[4][4] = scalar;
}

float5x5::float5x5(const float5& col0, const float5& col1, const float5& col2, const float5& col3, const float5& col4)
{
    columns[0] = col0;
    columns[1] = col1;
    columns[2] = col2;
    columns[3] = col3;
    columns[4] = col4;
}

float5x5::float5x5(const float5x5& other)
{
    columns[0] = other.columns[0];
    columns[1] = other.columns[1];
    columns[2] = other.columns[2];
    columns[3] = other.columns[3];
    columns[4] = other.columns[4];
}

float5x5::float5x5(const float6x6& other)
{
    columns[0] = float5{ other.columns[0][0], other.columns[0][1], other.columns[0][2], other.columns[0][3], other.columns[0][4] };
    columns[1] = float5{ other.columns[1][0], other.columns[1][1], other.columns[1][2], other.columns[1][3], other.columns[1][4] };
    columns[2] = float5{ other.columns[2][0], other.columns[2][1], other.columns[2][2], other.columns[2][3], other.columns[2][4] };
    columns[3] = float5{ other.columns[3][0], other.columns[3][1], other.columns[3][2], other.columns[3][3], other.columns[3][4] };
    columns[4] = float5{ other.columns[4][0], other.columns[4][1], other.columns[4][2], other.columns[4][3], other.columns[4][4] };
}

float5x5::float5x5(const float7x7& other)
{
    columns[0] = float5{ other.columns[0][0], other.columns[0][1], other.columns[0][2], other.columns[0][3], other.columns[0][4] };
    columns[1] = float5{ other.columns[1][0], other.columns[1][1], other.columns[1][2], other.columns[1][3], other.columns[1][4] };
    columns[2] = float5{ other.columns[2][0], other.columns[2][1], other.columns[2][2], other.columns[2][3], other.columns[2][4] };
    columns[3] = float5{ other.columns[3][0], other.columns[3][1], other.columns[3][2], other.columns[3][3], other.columns[3][4] };
    columns[4] = float5{ other.columns[4][0], other.columns[4][1], other.columns[4][2], other.columns[4][3], other.columns[4][4] };
}

const float6& float6x6::operator[](std::uint32_t index) const
{
    return columns[index];
}

float6& float6x6::operator[](std::uint32_t index)
{
    return columns[index];
}

float6x6::float6x6(float scalar)
{
    columns[0][0] = scalar;
    columns[1][1] = scalar;
    columns[2][2] = scalar;
    columns[3][3] = scalar;
    columns[4][4] = scalar;
    columns[5][5] = scalar;
}

float6x6::float6x6(const float6& col0, const float6& col1, const float6& col2, const float6& col3, const float6& col4, const float6& col5)
{
    columns[0] = col0;
    columns[1] = col1;
    columns[2] = col2;
    columns[3] = col3;
    columns[4] = col4;
    columns[5] = col5;
}

float6x6::float6x6(const float6x6& other)
{
    columns[0] = other.columns[0];
    columns[1] = other.columns[1];
    columns[2] = other.columns[2];
    columns[3] = other.columns[3];
    columns[4] = other.columns[4];
    columns[5] = other.columns[5];
}

float6x6::float6x6(const float7x7& other)
{
    columns[0] = float6{ other.columns[0][0], other.columns[0][1], other.columns[0][2], other.columns[0][3], other.columns[0][4], other.columns[0][5] };
    columns[1] = float6{ other.columns[1][0], other.columns[1][1], other.columns[1][2], other.columns[1][3], other.columns[1][4], other.columns[1][5] };
    columns[2] = float6{ other.columns[2][0], other.columns[2][1], other.columns[2][2], other.columns[2][3], other.columns[2][4], other.columns[2][5] };
    columns[3] = float6{ other.columns[3][0], other.columns[3][1], other.columns[3][2], other.columns[3][3], other.columns[3][4], other.columns[3][5] };
    columns[4] = float6{ other.columns[4][0], other.columns[4][1], other.columns[4][2], other.columns[4][3], other.columns[4][4], other.columns[4][5] };
    columns[5] = float6{ other.columns[5][0], other.columns[5][1], other.columns[5][2], other.columns[5][3], other.columns[5][4], other.columns[5][5] };
}

const float7& float7x7::operator[](std::uint32_t index) const
{
    return columns[index];
}

float7& float7x7::operator[](std::uint32_t index)
{
    return columns[index];
}

float7x7::float7x7(float scalar)
{
    columns[0][0] = scalar;
    columns[1][1] = scalar;
    columns[2][2] = scalar;
    columns[3][3] = scalar;
    columns[4][4] = scalar;
    columns[5][5] = scalar;
    columns[6][6] = scalar;
}

float7x7::float7x7(const float7& col0, const float7& col1, const float7& col2, const float7& col3, const float7& col4, const float7& col5, const float7& col6)
{
    columns[0] = col0;
    columns[1] = col1;
    columns[2] = col2;
    columns[3] = col3;
    columns[4] = col4;
    columns[5] = col5;
    columns[6] = col6;
}

float7x7::float7x7(const float7x7& other)
{
    columns[0] = other.columns[0];
    columns[1] = other.columns[1];
    columns[2] = other.columns[2];
    columns[3] = other.columns[3];
    columns[4] = other.columns[4];
    columns[5] = other.columns[5];
    columns[6] = other.columns[6];
}

float2x2 operator*(float s, const float2x2& x)
{
    return float2x2{
        { s * x[0][0], s * x[0][1] },
        { s * x[1][0], s * x[1][1] }
    };
}

float2x2 operator*(const float2x2& x, float s)
{
    return float2x2{
        { x[0][0] * s, x[0][1] * s },
        { x[1][0] * s, x[1][1] * s }
    };
}

float3x3 operator*(float s, const float3x3& x)
{
    return float3x3{
        { s * x[0][0], s * x[0][1], s * x[0][2] },
        { s * x[1][0], s * x[1][1], s * x[1][2] },
        { s * x[2][0], s * x[2][1], s * x[2][2] }
    };
}

float3x3 operator*(const float3x3& x, float s)
{
    return float3x3{
        { x[0][0] * s, x[0][1] * s, x[0][2] * s },
        { x[1][0] * s, x[1][1] * s, x[1][2] * s },
        { x[2][0] * s, x[2][1] * s, x[2][2] * s }
    };
}

float4x4 operator*(float s, const float4x4& x)
{
    return float4x4{
        { s * x[0][0], s * x[0][1], s * x[0][2], s * x[0][3] },
        { s * x[1][0], s * x[1][1], s * x[1][2], s * x[1][3] },
        { s * x[2][0], s * x[2][1], s * x[2][2], s * x[2][3] },
        { s * x[3][0], s * x[3][1], s * x[3][2], s * x[3][3] }
    };
}

float4x4 operator*(const float4x4& x, float s)
{
    return float4x4{
        { x[0][0] * s, x[0][1] * s, x[0][2] * s, x[0][3] * s },
        { x[1][0] * s, x[1][1] * s, x[1][2] * s, x[1][3] * s },
        { x[2][0] * s, x[2][1] * s, x[2][2] * s, x[2][3] * s },
        { x[3][0] * s, x[3][1] * s, x[3][2] * s, x[3][3] * s }
    };
}

float5x5 operator*(float s, const float5x5& x)
{
    return float5x5{
        { s * x[0][0], s * x[0][1], s * x[0][2], s * x[0][3], s * x[0][4] },
        { s * x[1][0], s * x[1][1], s * x[1][2], s * x[1][3], s * x[1][4] },
        { s * x[2][0], s * x[2][1], s * x[2][2], s * x[2][3], s * x[2][4] },
        { s * x[3][0], s * x[3][1], s * x[3][2], s * x[3][3], s * x[3][4] },
        { s * x[4][0], s * x[4][1], s * x[4][2], s * x[4][3], s * x[4][4] }
    };
}

float5x5 operator*(const float5x5& x, float s)
{
    return float5x5{
        { x[0][0] * s, x[0][1] * s, x[0][2] * s, x[0][3] * s, x[0][4] * s },
        { x[1][0] * s, x[1][1] * s, x[1][2] * s, x[1][3] * s, x[1][4] * s },
        { x[2][0] * s, x[2][1] * s, x[2][2] * s, x[2][3] * s, x[2][4] * s },
        { x[3][0] * s, x[3][1] * s, x[3][2] * s, x[3][3] * s, x[3][4] * s },
        { x[4][0] * s, x[4][1] * s, x[4][2] * s, x[4][3] * s, x[4][4] * s }
    };
}

float6x6 operator*(float s, const float6x6& x)
{
    return float6x6{
        { s * x[0][0], s * x[0][1], s * x[0][2], s * x[0][3], s * x[0][4], s * x[0][5] },
        { s * x[1][0], s * x[1][1], s * x[1][2], s * x[1][3], s * x[1][4], s * x[1][5] },
        { s * x[2][0], s * x[2][1], s * x[2][2], s * x[2][3], s * x[2][4], s * x[2][5] },
        { s * x[3][0], s * x[3][1], s * x[3][2], s * x[3][3], s * x[3][4], s * x[3][5] },
        { s * x[4][0], s * x[4][1], s * x[4][2], s * x[4][3], s * x[4][4], s * x[4][5] },
        { s * x[5][0], s * x[5][1], s * x[5][2], s * x[5][3], s * x[5][4], s * x[5][5] }
    };
}

float6x6 operator*(const float6x6& x, float s)
{
    return float6x6{
        { x[0][0] * s, x[0][1] * s, x[0][2] * s, x[0][3] * s, x[0][4] * s, x[0][5] * s },
        { x[1][0] * s, x[1][1] * s, x[1][2] * s, x[1][3] * s, x[1][4] * s, x[1][5] * s },
        { x[2][0] * s, x[2][1] * s, x[2][2] * s, x[2][3] * s, x[2][4] * s, x[2][5] * s },
        { x[3][0] * s, x[3][1] * s, x[3][2] * s, x[3][3] * s, x[3][4] * s, x[3][5] * s },
        { x[4][0] * s, x[4][1] * s, x[4][2] * s, x[4][3] * s, x[4][4] * s, x[4][5] * s },
        { x[5][0] * s, x[5][1] * s, x[5][2] * s, x[5][3] * s, x[5][4] * s, x[5][5] * s }
    };
}

float7x7 operator*(float s, const float7x7& x)
{
    return float7x7{
        { s * x[0][0], s * x[0][1], s * x[0][2], s * x[0][3], s * x[0][4], s * x[0][5], s * x[0][6] },
        { s * x[1][0], s * x[1][1], s * x[1][2], s * x[1][3], s * x[1][4], s * x[1][5], s * x[1][6] },
        { s * x[2][0], s * x[2][1], s * x[2][2], s * x[2][3], s * x[2][4], s * x[2][5], s * x[2][6] },
        { s * x[3][0], s * x[3][1], s * x[3][2], s * x[3][3], s * x[3][4], s * x[3][5], s * x[3][6] },
        { s * x[4][0], s * x[4][1], s * x[4][2], s * x[4][3], s * x[4][4], s * x[4][5], s * x[4][6] },
        { s * x[5][0], s * x[5][1], s * x[5][2], s * x[5][3], s * x[5][4], s * x[5][5], s * x[5][6] },
        { s * x[6][0], s * x[6][1], s * x[6][2], s * x[6][3], s * x[6][4], s * x[6][5], s * x[6][6] }
    };
}

float7x7 operator*(const float7x7& x, float s)
{
    return float7x7{
        { x[0][0] * s, x[0][1] * s, x[0][2] * s, x[0][3] * s, x[0][4] * s, x[0][5] * s, x[0][6] * s },
        { x[1][0] * s, x[1][1] * s, x[1][2] * s, x[1][3] * s, x[1][4] * s, x[1][5] * s, x[1][6] * s },
        { x[2][0] * s, x[2][1] * s, x[2][2] * s, x[2][3] * s, x[2][4] * s, x[2][5] * s, x[2][6] * s },
        { x[3][0] * s, x[3][1] * s, x[3][2] * s, x[3][3] * s, x[3][4] * s, x[3][5] * s, x[3][6] * s },
        { x[4][0] * s, x[4][1] * s, x[4][2] * s, x[4][3] * s, x[4][4] * s, x[4][5] * s, x[4][6] * s },
        { x[5][0] * s, x[5][1] * s, x[5][2] * s, x[5][3] * s, x[5][4] * s, x[5][5] * s, x[5][6] * s },
        { x[6][0] * s, x[6][1] * s, x[6][2] * s, x[6][3] * s, x[6][4] * s, x[6][5] * s, x[6][6] * s }
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

float5 operator*(const float5& v, const float5x5& x)
{
    return float5{
        x[0][0] * v[0] + x[0][1] * v[1] + x[0][2] * v[2] + x[0][3] * v[3] + x[0][4] * v[4],
        x[1][0] * v[0] + x[1][1] * v[1] + x[1][2] * v[2] + x[1][3] * v[3] + x[1][4] * v[4],
        x[2][0] * v[0] + x[2][1] * v[1] + x[2][2] * v[2] + x[2][3] * v[3] + x[2][4] * v[4],
        x[3][0] * v[0] + x[3][1] * v[1] + x[3][2] * v[2] + x[3][3] * v[3] + x[3][4] * v[4],
        x[4][0] * v[0] + x[4][1] * v[1] + x[4][2] * v[2] + x[4][3] * v[3] + x[4][4] * v[4]
    };
}

float5 operator*(const float5x5& x, const float5& v)
{
    return float5{
        x[0][0] * v[0] + x[1][0] * v[1] + x[2][0] * v[2] + x[3][0] * v[3] + x[4][0] * v[4],
        x[0][1] * v[0] + x[1][1] * v[1] + x[2][1] * v[2] + x[3][1] * v[3] + x[4][1] * v[4],
        x[0][2] * v[0] + x[1][2] * v[1] + x[2][2] * v[2] + x[3][2] * v[3] + x[4][2] * v[4],
        x[0][3] * v[0] + x[1][3] * v[1] + x[2][3] * v[2] + x[3][3] * v[3] + x[4][3] * v[4],
        x[0][4] * v[0] + x[1][4] * v[1] + x[2][4] * v[2] + x[3][4] * v[3] + x[4][4] * v[4]
    };
}

float6 operator*(const float6& v, const float6x6& x)
{
    return float6{
        x[0][0] * v[0] + x[0][1] * v[1] + x[0][2] * v[2] + x[0][3] * v[3] + x[0][4] * v[4] + x[0][5] * v[5],
        x[1][0] * v[0] + x[1][1] * v[1] + x[1][2] * v[2] + x[1][3] * v[3] + x[1][4] * v[4] + x[1][5] * v[5],
        x[2][0] * v[0] + x[2][1] * v[1] + x[2][2] * v[2] + x[2][3] * v[3] + x[2][4] * v[4] + x[2][5] * v[5],
        x[3][0] * v[0] + x[3][1] * v[1] + x[3][2] * v[2] + x[3][3] * v[3] + x[3][4] * v[4] + x[3][5] * v[5],
        x[4][0] * v[0] + x[4][1] * v[1] + x[4][2] * v[2] + x[4][3] * v[3] + x[4][4] * v[4] + x[4][5] * v[5],
        x[5][0] * v[0] + x[5][1] * v[1] + x[5][2] * v[2] + x[5][3] * v[3] + x[5][4] * v[4] + x[5][5] * v[5]
    };
}

float6 operator*(const float6x6& x, const float6& v)
{
    return float6{
        x[0][0] * v[0] + x[1][0] * v[1] + x[2][0] * v[2] + x[3][0] * v[3] + x[4][0] * v[4] + x[5][0] * v[5],
        x[0][1] * v[0] + x[1][1] * v[1] + x[2][1] * v[2] + x[3][1] * v[3] + x[4][1] * v[4] + x[5][1] * v[5],
        x[0][2] * v[0] + x[1][2] * v[1] + x[2][2] * v[2] + x[3][2] * v[3] + x[4][2] * v[4] + x[5][2] * v[5],
        x[0][3] * v[0] + x[1][3] * v[1] + x[2][3] * v[2] + x[3][3] * v[3] + x[4][3] * v[4] + x[5][3] * v[5],
        x[0][4] * v[0] + x[1][4] * v[1] + x[2][4] * v[2] + x[3][4] * v[3] + x[4][4] * v[4] + x[5][4] * v[5],
        x[0][5] * v[0] + x[1][5] * v[1] + x[2][5] * v[2] + x[3][5] * v[3] + x[4][5] * v[4] + x[5][5] * v[5]
    };
}

float7 operator*(const float7& v, const float7x7& x)
{
    return float7{
        x[0][0] * v[0] + x[0][1] * v[1] + x[0][2] * v[2] + x[0][3] * v[3] + x[0][4] * v[4] + x[0][5] * v[5] + x[0][6] * v[6],
        x[1][0] * v[0] + x[1][1] * v[1] + x[1][2] * v[2] + x[1][3] * v[3] + x[1][4] * v[4] + x[1][5] * v[5] + x[1][6] * v[6],
        x[2][0] * v[0] + x[2][1] * v[1] + x[2][2] * v[2] + x[2][3] * v[3] + x[2][4] * v[4] + x[2][5] * v[5] + x[2][6] * v[6],
        x[3][0] * v[0] + x[3][1] * v[1] + x[3][2] * v[2] + x[3][3] * v[3] + x[3][4] * v[4] + x[3][5] * v[5] + x[3][6] * v[6],
        x[4][0] * v[0] + x[4][1] * v[1] + x[4][2] * v[2] + x[4][3] * v[3] + x[4][4] * v[4] + x[4][5] * v[5] + x[4][6] * v[6],
        x[5][0] * v[0] + x[5][1] * v[1] + x[5][2] * v[2] + x[5][3] * v[3] + x[5][4] * v[4] + x[5][5] * v[5] + x[5][6] * v[6],
        x[6][0] * v[0] + x[6][1] * v[1] + x[6][2] * v[2] + x[6][3] * v[3] + x[6][4] * v[4] + x[6][5] * v[5] + x[6][6] * v[6]
    };
}

float7 operator*(const float7x7& x, const float7& v)
{
    return float7{
        x[0][0] * v[0] + x[1][0] * v[1] + x[2][0] * v[2] + x[3][0] * v[3] + x[4][0] * v[4] + x[5][0] * v[5] + x[6][0] * v[6],
        x[0][1] * v[0] + x[1][1] * v[1] + x[2][1] * v[2] + x[3][1] * v[3] + x[4][1] * v[4] + x[5][1] * v[5] + x[6][1] * v[6],
        x[0][2] * v[0] + x[1][2] * v[1] + x[2][2] * v[2] + x[3][2] * v[3] + x[4][2] * v[4] + x[5][2] * v[5] + x[6][2] * v[6],
        x[0][3] * v[0] + x[1][3] * v[1] + x[2][3] * v[2] + x[3][3] * v[3] + x[4][3] * v[4] + x[5][3] * v[5] + x[6][3] * v[6],
        x[0][4] * v[0] + x[1][4] * v[1] + x[2][4] * v[2] + x[3][4] * v[3] + x[4][4] * v[4] + x[5][4] * v[5] + x[6][4] * v[6],
        x[0][5] * v[0] + x[1][5] * v[1] + x[2][5] * v[2] + x[3][5] * v[3] + x[4][5] * v[4] + x[5][5] * v[5] + x[6][5] * v[6],
        x[0][6] * v[0] + x[1][6] * v[1] + x[2][6] * v[2] + x[3][6] * v[3] + x[4][6] * v[4] + x[5][6] * v[5] + x[6][6] * v[6]
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

float5x5 operator*(const float5x5& x, const float5x5& y)
{
    float5x5 result{};

    for (std::uint32_t c = 0u; c < 5u; c++)
    {
        for (std::uint32_t r = 0u; r < 5u; r++)
        {
            result.columns[c][r] = 0.0f;
            for (std::uint32_t k = 0u; k < 5u; k++)
            {
                result.columns[c][r] += x.columns[k][r] * y.columns[c][k];
            }
        }
    }

    return result;
}

float6x6 operator*(const float6x6& x, const float6x6& y)
{
    float6x6 result{};

    for (std::uint32_t c = 0u; c < 6u; c++)
    {
        for (std::uint32_t r = 0u; r < 6u; r++)
        {
            result.columns[c][r] = 0.0f;
            for (std::uint32_t k = 0u; k < 6u; k++)
            {
                result.columns[c][r] += x.columns[k][r] * y.columns[c][k];
            }
        }
    }

    return result;
}

float7x7 operator*(const float7x7& x, const float7x7& y)
{
    float7x7 result{};

    for (std::uint32_t c = 0u; c < 7u; c++)
    {
        for (std::uint32_t r = 0u; r < 7u; r++)
        {
            result.columns[c][r] = 0.0f;
            for (std::uint32_t k = 0u; k < 7u; k++)
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

float5 mul(const float5& v, const float5x5& x)
{
    return v * x;
}

float5 mul(const float5x5& x, const float5& v)
{
    return x * v;
}

float6 mul(const float6& v, const float6x6& x)
{
    return v * x;
}

float6 mul(const float6x6& x, const float6& v)
{
    return x * v;
}

float7 mul(const float7& v, const float7x7& x)
{
    return v * x;
}

float7 mul(const float7x7& x, const float7& v)
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

float4x4 submatrix(const float5x5& x, std::uint32_t col, std::uint32_t row)
{
    float4x4 result{ 0.0f };

    std::uint32_t subi{ 0u };
    for (std::uint32_t i = 0u; i < 5u; i++)
    {
        if (i == col)
        {
            continue;
        }
        std::uint32_t subj{ 0u };
        for (std::uint32_t j = 0u; j < 5u; j++)
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

float5x5 submatrix(const float6x6& x, std::uint32_t col, std::uint32_t row)
{
    float5x5 result{ 0.0f };

    std::uint32_t subi{ 0u };
    for (std::uint32_t i = 0u; i < 6u; i++)
    {
        if (i == col)
        {
            continue;
        }
        std::uint32_t subj{ 0u };
        for (std::uint32_t j = 0u; j < 6u; j++)
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

float6x6 submatrix(const float7x7& x, std::uint32_t col, std::uint32_t row)
{
    float6x6 result{ 0.0f };

    std::uint32_t subi{ 0u };
    for (std::uint32_t i = 0u; i < 7u; i++)
    {
        if (i == col)
        {
            continue;
        }
        std::uint32_t subj{ 0u };
        for (std::uint32_t j = 0u; j < 7u; j++)
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
    return x[0][0] * determinant(submatrix(x, 0, 0)) - x[0][1] * determinant(submatrix(x, 0, 1)) + x[0][2] * determinant(submatrix(x, 0, 2)) - x[0][3] * determinant(submatrix(x, 0, 3));
}

float determinant(const float5x5& x)
{
    return x[0][0] * determinant(submatrix(x, 0, 0)) - x[0][1] * determinant(submatrix(x, 0, 1)) + x[0][2] * determinant(submatrix(x, 0, 2)) - x[0][3] * determinant(submatrix(x, 0, 3)) + x[0][4] * determinant(submatrix(x, 0, 4));
}

float determinant(const float6x6& x)
{
    return x[0][0] * determinant(submatrix(x, 0, 0)) - x[0][1] * determinant(submatrix(x, 0, 1)) + x[0][2] * determinant(submatrix(x, 0, 2)) - x[0][3] * determinant(submatrix(x, 0, 3)) + x[0][4] * determinant(submatrix(x, 0, 4)) - x[0][5] * determinant(submatrix(x, 0, 5));
}

float determinant(const float7x7& x)
{
    return x[0][0] * determinant(submatrix(x, 0, 0)) - x[0][1] * determinant(submatrix(x, 0, 1)) + x[0][2] * determinant(submatrix(x, 0, 2)) - x[0][3] * determinant(submatrix(x, 0, 3)) + x[0][4] * determinant(submatrix(x, 0, 4)) - x[0][5] * determinant(submatrix(x, 0, 5)) + x[0][6] * determinant(submatrix(x, 0, 6));
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

float minor(const float5x5& x, std::uint32_t col, std::uint32_t row)
{
    return determinant(submatrix(x, col, row));
}

float minor(const float6x6& x, std::uint32_t col, std::uint32_t row)
{
    return determinant(submatrix(x, col, row));
}

float minor(const float7x7& x, std::uint32_t col, std::uint32_t row)
{
    return determinant(submatrix(x, col, row));
}

float2x2 transpose(const float2x2& x)
{
    return float2x2{
        { x[0][0], x[1][0] },
        { x[0][1], x[1][1] }
    };
}

float3x3 transpose(const float3x3& x)
{
    return float3x3{
        { x[0][0], x[1][0], x[2][0] },
        { x[0][1], x[1][1], x[2][1] },
        { x[0][2], x[1][2], x[2][2] }
    };
}

float4x4 transpose(const float4x4& x)
{
    return float4x4{
        { x[0][0], x[1][0], x[2][0], x[3][0] },
        { x[0][1], x[1][1], x[2][1], x[3][1] },
        { x[0][2], x[1][2], x[2][2], x[3][2] },
        { x[0][3], x[1][3], x[2][3], x[3][3] }
    };
}

float5x5 transpose(const float5x5& x)
{
    return float5x5{
        { x[0][0], x[1][0], x[2][0], x[3][0], x[4][0] },
        { x[0][1], x[1][1], x[2][1], x[3][1], x[4][1] },
        { x[0][2], x[1][2], x[2][2], x[3][2], x[4][2] },
        { x[0][3], x[1][3], x[2][3], x[3][3], x[4][3] },
        { x[0][4], x[1][4], x[2][4], x[3][4], x[4][4] }
    };
}

float6x6 transpose(const float6x6& x)
{
    return float6x6{
        { x[0][0], x[1][0], x[2][0], x[3][0], x[4][0], x[5][0] },
        { x[0][1], x[1][1], x[2][1], x[3][1], x[4][1], x[5][1] },
        { x[0][2], x[1][2], x[2][2], x[3][2], x[4][2], x[5][2] },
        { x[0][3], x[1][3], x[2][3], x[3][3], x[4][3], x[5][3] },
        { x[0][4], x[1][4], x[2][4], x[3][4], x[4][4], x[5][4] },
        { x[0][5], x[1][5], x[2][5], x[3][5], x[4][5], x[5][5] }
    };
}

float7x7 transpose(const float7x7& x)
{
    return float7x7{
        { x[0][0], x[1][0], x[2][0], x[3][0], x[4][0], x[5][0], x[6][0] },
        { x[0][1], x[1][1], x[2][1], x[3][1], x[4][1], x[5][1], x[6][1] },
        { x[0][2], x[1][2], x[2][2], x[3][2], x[4][2], x[5][2], x[6][2] },
        { x[0][3], x[1][3], x[2][3], x[3][3], x[4][3], x[5][3], x[6][3] },
        { x[0][4], x[1][4], x[2][4], x[3][4], x[4][4], x[5][4], x[6][4] },
        { x[0][5], x[1][5], x[2][5], x[3][5], x[4][5], x[5][5], x[6][5] },
        { x[0][6], x[1][6], x[2][6], x[3][6], x[4][6], x[5][6], x[6][6] }
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

float5x5 cofactor(const float5x5& x)
{
    float5x5 result{ 0.0f };

    for (std::uint32_t i = 0u; i < 5u; i++)
    {
        for (std::uint32_t j = 0u; j < 5u; j++)
        {
            result[i][j] = std::pow(-1.0f, (float)(i + j)) * minor(x, i, j);
        }
    }

    return result;
}

float6x6 cofactor(const float6x6& x)
{
    float6x6 result{ 0.0f };

    for (std::uint32_t i = 0u; i < 6u; i++)
    {
        for (std::uint32_t j = 0u; j < 6u; j++)
        {
            result[i][j] = std::pow(-1.0f, (float)(i + j)) * minor(x, i, j);
        }
    }

    return result;
}

float7x7 cofactor(const float7x7& x)
{
    float7x7 result{ 0.0f };

    for (std::uint32_t i = 0u; i < 7u; i++)
    {
        for (std::uint32_t j = 0u; j < 7u; j++)
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

float5x5 adjoint(const float5x5& x)
{
    return transpose(cofactor(x));
}

float6x6 adjoint(const float6x6& x)
{
    return transpose(cofactor(x));
}

float7x7 adjoint(const float7x7& x)
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

float5x5 inverse(const float5x5& x)
{
    auto inv_det = 1.0f / determinant(x);

    return inv_det * adjoint(x);
}

float6x6 inverse(const float6x6& x)
{
    auto inv_det = 1.0f / determinant(x);

    return inv_det * adjoint(x);
}

float7x7 inverse(const float7x7& x)
{
    auto inv_det = 1.0f / determinant(x);

    return inv_det * adjoint(x);
}
