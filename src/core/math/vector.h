#ifndef CORE_MATH_VECTOR_H_
#define CORE_MATH_VECTOR_H_

#include <cstdint>

struct float4;
struct float3;

struct float2
{
    float x{ 0.0f };
    float y{ 0.0f };

    const float& operator[](std::uint32_t index) const;

    float& operator[](std::uint32_t index);

    float2() = default;

    explicit float2(float scalar);

    float2(float x, float y);

    float2(const float2& other);

    explicit float2(const float3& other);

    explicit float2(const float4& other);
};

struct float3
{
    float x{ 0.0f };
    float y{ 0.0f };
    float z{ 0.0f };

    const float& operator[](std::uint32_t index) const;

    float& operator[](std::uint32_t index);

    float3() = default;

    explicit float3(float scalar);

    float3(float x, float y, float z);

    float3(const float2& v, float z);

    float3(float x, const float2& v);

    float3(const float3& other);

    explicit float3(const float4& other);
};

struct float4
{
    float x{ 0.0f };
    float y{ 0.0f };
    float z{ 0.0f };
    float w{ 0.0f };

    const float& operator[](std::uint32_t index) const;

    float& operator[](std::uint32_t index);

    float4() = default;

    explicit float4(float scalar);

    float4(float x, float y, float z, float w);

    float4(const float2& v, float z, float w);

    float4(float x, const float2& v, float w);

    float4(float x, float y, const float2& v);

    float4(float x, const float3& v);

    float4(const float3& v, float w);

    float4(const float4& other);
};

float2 operator*(float s, const float2& x);

float2 operator*(const float2& x, float s);

float3 operator*(float s, const float3& x);

float3 operator*(const float3& x, float s);

float4 operator*(float s, const float4& x);

float4 operator*(const float4& x, float s);

float2 operator/(float s, const float2& x);

float2 operator/(const float2& x, float s);

float3 operator/(float s, const float3& x);

float3 operator/(const float3& x, float s);

float4 operator/(float s, const float4& x);

float4 operator/(const float4& x, float s);

float2 operator+(const float2& x, const float2& y);

float3 operator+(const float3& x, const float3& y);

float4 operator+(const float4& x, const float4& y);

float2 operator-(const float2& x, const float2& y);

float3 operator-(const float3& x, const float3& y);

float4 operator-(const float4& x, const float4& y);

float2 operator*(const float2& x, const float2& y);

float3 operator*(const float3& x, const float3& y);

float4 operator*(const float4& x, const float4& y);

float2 operator/(const float2& x, const float2& y);

float3 operator/(const float3& x, const float3& y);

float4 operator/(const float4& x, const float4& y);

float2 operator+(const float2& x);

float3 operator+(const float3& x);

float4 operator+(const float4& x);

float2 operator-(const float2& x);

float3 operator-(const float3& x);

float4 operator-(const float4& x);

float dot(const float2& x, const float2& y);

float dot(const float3& x, const float3& y);

float dot(const float4& x, const float4& y);

float length(const float2& x);

float length(const float3& x);

float length(const float4& x);

float distance(const float2& x, const float2& y);

float distance(const float3& x, const float3& y);

float distance(const float4& x, const float4& y);

float3 cross(const float3& a, const float3& b);

float2 normalize(const float2& x);

float3 normalize(const float3& x);

float4 normalize(const float4& x);

#endif /* CORE_MATH_VECTOR_H_ */
