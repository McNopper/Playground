#ifndef CORE_MATH_MATRIX_H_
#define CORE_MATH_MATRIX_H_

#include <cstdint>

#include "vector.h"

struct float4x4;
struct float3x3;

struct float2x2
{
    float2 columns[2];

    const float2& operator[](std::uint32_t index) const;

    float2& operator[](std::uint32_t index);

    float2x2() = default;

    explicit float2x2(float scalar);

    float2x2(const float2& col0, const float2& col1);

    float2x2(const float2x2& other);

    explicit float2x2(const float3x3& other);

    explicit float2x2(const float4x4& other);
};

struct float3x3
{
    float3 columns[3];

    const float3& operator[](std::uint32_t index) const;

    float3& operator[](std::uint32_t index);

    float3x3() = default;

    explicit float3x3(float scalar);

    float3x3(const float3& col0, const float3& col1, const float3& col2);

    explicit float3x3(const float2x2& other);

    float3x3(const float3x3& other);

    explicit float3x3(const float4x4& other);
};

struct float4x4
{
    float4 columns[4];

    const float4& operator[](std::uint32_t index) const;

    float4& operator[](std::uint32_t index);

    float4x4() = default;

    explicit float4x4(float scalar);

    float4x4(const float4& col0, const float4& col1, const float4& col2, const float4& col3);

    explicit float4x4(const float2x2& other);

    explicit float4x4(const float3x3& other);

    float4x4(const float4x4& other);
};

float2x2 operator*(float s, const float2x2& x);

float2x2 operator*(const float2x2& x, float s);

float3x3 operator*(float s, const float3x3& x);

float3x3 operator*(const float3x3& x, float s);

float4x4 operator*(float s, const float4x4& x);

float4x4 operator*(const float4x4& x, float s);

float2 operator*(const float2& v, const float2x2& x);

float2 operator*(const float2x2& x, const float2& v);

float3 operator*(const float3& v, const float3x3& x);

float3 operator*(const float3x3& x, const float3& v);

float4 operator*(const float4& v, const float4x4& x);

float4 operator*(const float4x4& x, const float4& v);

float2x2 operator*(const float2x2& x, const float2x2& y);

float3x3 operator*(const float3x3& x, const float3x3& y);

float4x4 operator*(const float4x4& x, const float4x4& y);

// Functions

float2 mul(const float2& v, const float2x2& x);

float2 mul(const float2x2& x, const float2& v);

float3 mul(const float3& v, const float3x3& x);

float3 mul(const float3x3& x, const float3& v);

float4 mul(const float4& v, const float4x4& x);

float4 mul(const float4x4& x, const float4& v);

float2x2 submatrix(const float3x3& x, std::uint32_t col, std::uint32_t row);

float3x3 submatrix(const float4x4& x, std::uint32_t col, std::uint32_t row);

float determinant(const float2x2& x);

float determinant(const float3x3& x);

float determinant(const float4x4& x);

float minor(const float2x2& x, std::uint32_t col, std::uint32_t row);

float minor(const float3x3& x, std::uint32_t col, std::uint32_t row);

float minor(const float4x4& x, std::uint32_t col, std::uint32_t row);

float2x2 transpose(const float2x2& x);

float3x3 transpose(const float3x3& x);

float4x4 transpose(const float4x4& x);

float2x2 cofactor(const float2x2& x);

float3x3 cofactor(const float3x3& x);

float4x4 cofactor(const float4x4& x);

float2x2 adjoint(const float2x2& x);

float3x3 adjoint(const float3x3& x);

float4x4 adjoint(const float4x4& x);

float2x2 inverse(const float2x2& x);

float3x3 inverse(const float3x3& x);

float4x4 inverse(const float4x4& x);

#endif /* CORE_MATH_MATRIX_H_ */
