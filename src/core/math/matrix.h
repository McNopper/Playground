#ifndef CORE_MATH_MATRIX_H_
#define CORE_MATH_MATRIX_H_

#include <cstdint>

#include "vector.h"

struct float4x4;
struct float3x3;
struct float5x5;
struct float6x6;
struct float7x7;

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

struct float5x5
{
    float5 columns[5];

    const float5& operator[](std::uint32_t index) const;

    float5& operator[](std::uint32_t index);

    float5x5() = default;

    explicit float5x5(float scalar);

    float5x5(const float5& col0, const float5& col1, const float5& col2, const float5& col3, const float5& col4);

    float5x5(const float5x5& other);

    explicit float5x5(const float6x6& other);

    explicit float5x5(const float7x7& other);
};

struct float6x6
{
    float6 columns[6];

    const float6& operator[](std::uint32_t index) const;

    float6& operator[](std::uint32_t index);

    float6x6() = default;

    explicit float6x6(float scalar);

    float6x6(const float6& col0, const float6& col1, const float6& col2, const float6& col3, const float6& col4, const float6& col5);

    float6x6(const float6x6& other);

    explicit float6x6(const float7x7& other);
};

struct float7x7
{
    float7 columns[7];

    const float7& operator[](std::uint32_t index) const;

    float7& operator[](std::uint32_t index);

    float7x7() = default;

    explicit float7x7(float scalar);

    float7x7(const float7& col0, const float7& col1, const float7& col2, const float7& col3, const float7& col4, const float7& col5, const float7& col6);

    float7x7(const float7x7& other);
};

float2x2 operator*(float s, const float2x2& x);

float2x2 operator*(const float2x2& x, float s);

float3x3 operator*(float s, const float3x3& x);

float3x3 operator*(const float3x3& x, float s);

float4x4 operator*(float s, const float4x4& x);

float4x4 operator*(const float4x4& x, float s);

float5x5 operator*(float s, const float5x5& x);

float5x5 operator*(const float5x5& x, float s);

float6x6 operator*(float s, const float6x6& x);

float6x6 operator*(const float6x6& x, float s);

float7x7 operator*(float s, const float7x7& x);

float7x7 operator*(const float7x7& x, float s);

float2 operator*(const float2& v, const float2x2& x);

float2 operator*(const float2x2& x, const float2& v);

float3 operator*(const float3& v, const float3x3& x);

float3 operator*(const float3x3& x, const float3& v);

float4 operator*(const float4& v, const float4x4& x);

float4 operator*(const float4x4& x, const float4& v);

float5 operator*(const float5& v, const float5x5& x);

float5 operator*(const float5x5& x, const float5& v);

float6 operator*(const float6& v, const float6x6& x);

float6 operator*(const float6x6& x, const float6& v);

float7 operator*(const float7& v, const float7x7& x);

float7 operator*(const float7x7& x, const float7& v);

float2x2 operator*(const float2x2& x, const float2x2& y);

float3x3 operator*(const float3x3& x, const float3x3& y);

float4x4 operator*(const float4x4& x, const float4x4& y);

float5x5 operator*(const float5x5& x, const float5x5& y);

float6x6 operator*(const float6x6& x, const float6x6& y);

float7x7 operator*(const float7x7& x, const float7x7& y);

// Functions

float2 mul(const float2& v, const float2x2& x);

float2 mul(const float2x2& x, const float2& v);

float3 mul(const float3& v, const float3x3& x);

float3 mul(const float3x3& x, const float3& v);

float4 mul(const float4& v, const float4x4& x);

float4 mul(const float4x4& x, const float4& v);

float5 mul(const float5& v, const float5x5& x);

float5 mul(const float5x5& x, const float5& v);

float6 mul(const float6& v, const float6x6& x);

float6 mul(const float6x6& x, const float6& v);

float7 mul(const float7& v, const float7x7& x);

float7 mul(const float7x7& x, const float7& v);

float2x2 submatrix(const float3x3& x, std::uint32_t col, std::uint32_t row);

float3x3 submatrix(const float4x4& x, std::uint32_t col, std::uint32_t row);

float4x4 submatrix(const float5x5& x, std::uint32_t col, std::uint32_t row);

float5x5 submatrix(const float6x6& x, std::uint32_t col, std::uint32_t row);

float6x6 submatrix(const float7x7& x, std::uint32_t col, std::uint32_t row);

float determinant(const float2x2& x);

float determinant(const float3x3& x);

float determinant(const float4x4& x);

float determinant(const float5x5& x);

float determinant(const float6x6& x);

float determinant(const float7x7& x);

float minor(const float2x2& x, std::uint32_t col, std::uint32_t row);

float minor(const float3x3& x, std::uint32_t col, std::uint32_t row);

float minor(const float4x4& x, std::uint32_t col, std::uint32_t row);

float minor(const float5x5& x, std::uint32_t col, std::uint32_t row);

float minor(const float6x6& x, std::uint32_t col, std::uint32_t row);

float minor(const float7x7& x, std::uint32_t col, std::uint32_t row);

float2x2 transpose(const float2x2& x);

float3x3 transpose(const float3x3& x);

float4x4 transpose(const float4x4& x);

float5x5 transpose(const float5x5& x);

float6x6 transpose(const float6x6& x);

float7x7 transpose(const float7x7& x);

float2x2 cofactor(const float2x2& x);

float3x3 cofactor(const float3x3& x);

float4x4 cofactor(const float4x4& x);

float5x5 cofactor(const float5x5& x);

float6x6 cofactor(const float6x6& x);

float7x7 cofactor(const float7x7& x);

float2x2 adjoint(const float2x2& x);

float3x3 adjoint(const float3x3& x);

float4x4 adjoint(const float4x4& x);

float5x5 adjoint(const float5x5& x);

float6x6 adjoint(const float6x6& x);

float7x7 adjoint(const float7x7& x);

float2x2 inverse(const float2x2& x);

float3x3 inverse(const float3x3& x);

float4x4 inverse(const float4x4& x);

float5x5 inverse(const float5x5& x);

float6x6 inverse(const float6x6& x);

float7x7 inverse(const float7x7& x);

#endif /* CORE_MATH_MATRIX_H_ */
