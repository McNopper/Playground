#ifndef CORE_MATH_VECTOR_H_
#define CORE_MATH_VECTOR_H_

#include <cstdint>

struct float4;
struct float3;
struct float5;
struct float6;
struct float7;

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

struct float5
{
    float x{ 0.0f };
    float y{ 0.0f };
    float z{ 0.0f };
    float w{ 0.0f };
    float v{ 0.0f };

    const float& operator[](std::uint32_t index) const;

    float& operator[](std::uint32_t index);

    float5() = default;

    explicit float5(float scalar);

    float5(float x, float y, float z, float w, float v);

    float5(const float4& vec, float v);

    float5(const float5& other);

    explicit float5(const float6& other);

    explicit float5(const float7& other);
};

struct float6
{
    float x{ 0.0f };
    float y{ 0.0f };
    float z{ 0.0f };
    float w{ 0.0f };
    float v{ 0.0f };
    float u{ 0.0f };

    const float& operator[](std::uint32_t index) const;

    float& operator[](std::uint32_t index);

    float6() = default;

    explicit float6(float scalar);

    float6(float x, float y, float z, float w, float v, float u);

    float6(const float5& vec, float u);

    float6(const float6& other);

    explicit float6(const float7& other);
};

struct float7
{
    float x{ 0.0f };
    float y{ 0.0f };
    float z{ 0.0f };
    float w{ 0.0f };
    float v{ 0.0f };
    float u{ 0.0f };
    float t{ 0.0f };

    const float& operator[](std::uint32_t index) const;

    float& operator[](std::uint32_t index);

    float7() = default;

    explicit float7(float scalar);

    float7(float x, float y, float z, float w, float v, float u, float t);

    float7(const float6& vec, float t);

    float7(const float7& other);
};

float2 operator*(float s, const float2& x);

float2 operator*(const float2& x, float s);

float3 operator*(float s, const float3& x);

float3 operator*(const float3& x, float s);

float4 operator*(float s, const float4& x);

float4 operator*(const float4& x, float s);

float5 operator*(float s, const float5& x);

float5 operator*(const float5& x, float s);

float6 operator*(float s, const float6& x);

float6 operator*(const float6& x, float s);

float7 operator*(float s, const float7& x);

float7 operator*(const float7& x, float s);

float2 operator/(float s, const float2& x);

float2 operator/(const float2& x, float s);

float3 operator/(float s, const float3& x);

float3 operator/(const float3& x, float s);

float4 operator/(float s, const float4& x);

float4 operator/(const float4& x, float s);

float5 operator/(float s, const float5& x);

float5 operator/(const float5& x, float s);

float6 operator/(float s, const float6& x);

float6 operator/(const float6& x, float s);

float7 operator/(float s, const float7& x);

float7 operator/(const float7& x, float s);

float2 operator+(const float2& x, const float2& y);

float3 operator+(const float3& x, const float3& y);

float4 operator+(const float4& x, const float4& y);

float5 operator+(const float5& x, const float5& y);

float6 operator+(const float6& x, const float6& y);

float7 operator+(const float7& x, const float7& y);

float2 operator-(const float2& x, const float2& y);

float3 operator-(const float3& x, const float3& y);

float4 operator-(const float4& x, const float4& y);

float5 operator-(const float5& x, const float5& y);

float6 operator-(const float6& x, const float6& y);

float7 operator-(const float7& x, const float7& y);

float2 operator*(const float2& x, const float2& y);

float3 operator*(const float3& x, const float3& y);

float4 operator*(const float4& x, const float4& y);

float5 operator*(const float5& x, const float5& y);

float6 operator*(const float6& x, const float6& y);

float7 operator*(const float7& x, const float7& y);

float2 operator/(const float2& x, const float2& y);

float3 operator/(const float3& x, const float3& y);

float4 operator/(const float4& x, const float4& y);

float5 operator/(const float5& x, const float5& y);

float6 operator/(const float6& x, const float6& y);

float7 operator/(const float7& x, const float7& y);

float2 operator+(const float2& x);

float3 operator+(const float3& x);

float4 operator+(const float4& x);

float5 operator+(const float5& x);

float6 operator+(const float6& x);

float7 operator+(const float7& x);

float2 operator-(const float2& x);

float3 operator-(const float3& x);

float4 operator-(const float4& x);

float5 operator-(const float5& x);

float6 operator-(const float6& x);

float7 operator-(const float7& x);

float dot(const float2& x, const float2& y);

float dot(const float3& x, const float3& y);

float dot(const float4& x, const float4& y);

float dot(const float5& x, const float5& y);

float dot(const float6& x, const float6& y);

float dot(const float7& x, const float7& y);

float length(const float2& x);

float length(const float3& x);

float length(const float4& x);

float length(const float5& x);

float length(const float6& x);

float length(const float7& x);

float distance(const float2& x, const float2& y);

float distance(const float3& x, const float3& y);

float distance(const float4& x, const float4& y);

float distance(const float5& x, const float5& y);

float distance(const float6& x, const float6& y);

float distance(const float7& x, const float7& y);

float3 cross(const float3& a, const float3& b);

float2 normalize(const float2& x);

float3 normalize(const float3& x);

float4 normalize(const float4& x);

float5 normalize(const float5& x);

float6 normalize(const float6& x);

float7 normalize(const float7& x);

#endif /* CORE_MATH_VECTOR_H_ */
