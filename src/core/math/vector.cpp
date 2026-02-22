#include "vector.h"

#include <cmath>

const float& float2::operator[](std::uint32_t index) const
{
    return *(reinterpret_cast<const float*>(this) + index);
}

float& float2::operator[](std::uint32_t index)
{
    return *(reinterpret_cast<float*>(this) + index);
}

float2::float2(float scalar) :
    x{ scalar }, y{ scalar }
{
}

float2::float2(float x, float y)
{
    this->x = x;
    this->y = y;
}

float2::float2(const float2& other)
{
    x = other.x;
    y = other.y;
}

float2::float2(const float3& other)
{
    x = other.x;
    y = other.y;
}

float2::float2(const float4& other)
{
    x = other.x;
    y = other.y;
}

const float& float3::operator[](std::uint32_t index) const
{
    return *(reinterpret_cast<const float*>(this) + index);
}

float& float3::operator[](std::uint32_t index)
{
    return *(reinterpret_cast<float*>(this) + index);
}

float3::float3(float scalar) :
    x{ scalar }, y{ scalar }, z{ scalar }
{
}

float3::float3(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

float3::float3(const float2& v, float z)
{
    this->x = v[0];
    this->y = v[1];
    this->z = z;
}

float3::float3(float x, const float2& v)
{
    this->x = x;
    this->y = v[0];
    this->z = v[1];
}

float3::float3(const float3& other)
{
    x = other.x;
    y = other.y;
    z = other.z;
}

float3::float3(const float4& other)
{
    x = other.x;
    y = other.y;
    z = other.z;
}

const float& float4::operator[](std::uint32_t index) const
{
    return *(reinterpret_cast<const float*>(this) + index);
}

float& float4::operator[](std::uint32_t index)
{
    return *(reinterpret_cast<float*>(this) + index);
}

float4::float4(float scalar) :
    x{ scalar }, y{ scalar }, z{ scalar }, w{ scalar }
{
}

float4::float4(float x, float y, float z, float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

float4::float4(const float2& v, float z, float w)
{
    this->x = v[0];
    this->y = v[1];
    this->z = z;
    this->w = w;
}

float4::float4(float x, const float2& v, float w)
{
    this->x = x;
    this->y = v[0];
    this->z = v[1];
    this->w = w;
}

float4::float4(float x, float y, const float2& v)
{
    this->x = x;
    this->y = y;
    this->z = v[0];
    this->w = v[1];
}

float4::float4(float x, const float3& v)
{
    this->x = x;
    this->y = v[0];
    this->z = v[1];
    this->w = v[2];
}

float4::float4(const float3& v, float w)
{
    this->x = v[0];
    this->y = v[1];
    this->z = v[2];
    this->w = w;
}

float4::float4(const float4& other)
{
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;
}

const float& float5::operator[](std::uint32_t index) const
{
    return *(reinterpret_cast<const float*>(this) + index);
}

float& float5::operator[](std::uint32_t index)
{
    return *(reinterpret_cast<float*>(this) + index);
}

float5::float5(float scalar) :
    x{ scalar }, y{ scalar }, z{ scalar }, w{ scalar }, v{ scalar }
{
}

float5::float5(float x, float y, float z, float w, float v)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
    this->v = v;
}

float5::float5(const float4& vec, float v)
{
    this->x = vec[0];
    this->y = vec[1];
    this->z = vec[2];
    this->w = vec[3];
    this->v = v;
}

float5::float5(const float5& other)
{
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;
    v = other.v;
}

float5::float5(const float6& other)
{
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;
    v = other.v;
}

float5::float5(const float7& other)
{
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;
    v = other.v;
}

const float& float6::operator[](std::uint32_t index) const
{
    return *(reinterpret_cast<const float*>(this) + index);
}

float& float6::operator[](std::uint32_t index)
{
    return *(reinterpret_cast<float*>(this) + index);
}

float6::float6(float scalar) :
    x{ scalar }, y{ scalar }, z{ scalar }, w{ scalar }, v{ scalar }, u{ scalar }
{
}

float6::float6(float x, float y, float z, float w, float v, float u)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
    this->v = v;
    this->u = u;
}

float6::float6(const float5& vec, float u)
{
    this->x = vec[0];
    this->y = vec[1];
    this->z = vec[2];
    this->w = vec[3];
    this->v = vec[4];
    this->u = u;
}

float6::float6(const float6& other)
{
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;
    v = other.v;
    u = other.u;
}

float6::float6(const float7& other)
{
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;
    v = other.v;
    u = other.u;
}

const float& float7::operator[](std::uint32_t index) const
{
    return *(reinterpret_cast<const float*>(this) + index);
}

float& float7::operator[](std::uint32_t index)
{
    return *(reinterpret_cast<float*>(this) + index);
}

float7::float7(float scalar) :
    x{ scalar }, y{ scalar }, z{ scalar }, w{ scalar }, v{ scalar }, u{ scalar }, t{ scalar }
{
}

float7::float7(float x, float y, float z, float w, float v, float u, float t)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
    this->v = v;
    this->u = u;
    this->t = t;
}

float7::float7(const float6& vec, float t)
{
    this->x = vec[0];
    this->y = vec[1];
    this->z = vec[2];
    this->w = vec[3];
    this->v = vec[4];
    this->u = vec[5];
    this->t = t;
}

float7::float7(const float7& other)
{
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;
    v = other.v;
    u = other.u;
    t = other.t;
}

float2 operator*(float s, const float2& x)
{
    return float2{
        s * x[0],
        s * x[1]
    };
}

float2 operator*(const float2& x, float s)
{
    return float2{
        x[0] * s,
        x[1] * s
    };
}

float3 operator*(float s, const float3& x)
{
    return float3{
        s * x[0],
        s * x[1],
        s * x[2]
    };
}

float3 operator*(const float3& x, float s)
{
    return float3{
        x[0] * s,
        x[1] * s,
        x[2] * s
    };
}

float4 operator*(float s, const float4& x)
{
    return float4{
        s * x[0],
        s * x[1],
        s * x[2],
        s * x[3]
    };
}

float4 operator*(const float4& x, float s)
{
    return float4{
        x[0] * s,
        x[1] * s,
        x[2] * s,
        x[3] * s
    };
}

float5 operator*(float s, const float5& x)
{
    return float5{
        s * x[0],
        s * x[1],
        s * x[2],
        s * x[3],
        s * x[4]
    };
}

float5 operator*(const float5& x, float s)
{
    return float5{
        x[0] * s,
        x[1] * s,
        x[2] * s,
        x[3] * s,
        x[4] * s
    };
}

float6 operator*(float s, const float6& x)
{
    return float6{
        s * x[0],
        s * x[1],
        s * x[2],
        s * x[3],
        s * x[4],
        s * x[5]
    };
}

float6 operator*(const float6& x, float s)
{
    return float6{
        x[0] * s,
        x[1] * s,
        x[2] * s,
        x[3] * s,
        x[4] * s,
        x[5] * s
    };
}

float7 operator*(float s, const float7& x)
{
    return float7{
        s * x[0],
        s * x[1],
        s * x[2],
        s * x[3],
        s * x[4],
        s * x[5],
        s * x[6]
    };
}

float7 operator*(const float7& x, float s)
{
    return float7{
        x[0] * s,
        x[1] * s,
        x[2] * s,
        x[3] * s,
        x[4] * s,
        x[5] * s,
        x[6] * s
    };
}

float2 operator/(float s, const float2& x)
{
    return float2{
        s / x[0],
        s / x[1]
    };
}

float2 operator/(const float2& x, float s)
{
    return float2{
        x[0] / s,
        x[1] / s
    };
}

float3 operator/(float s, const float3& x)
{
    return float3{
        s / x[0],
        s / x[1],
        s / x[2]
    };
}

float3 operator/(const float3& x, float s)
{
    return float3{
        x[0] / s,
        x[1] / s,
        x[2] / s
    };
}

float4 operator/(float s, const float4& x)
{
    return float4{
        s / x[0],
        s / x[1],
        s / x[2],
        s / x[3]
    };
}

float4 operator/(const float4& x, float s)
{
    return float4{
        x[0] / s,
        x[1] / s,
        x[2] / s,
        x[3] / s
    };
}

float5 operator/(float s, const float5& x)
{
    return float5{
        s / x[0],
        s / x[1],
        s / x[2],
        s / x[3],
        s / x[4]
    };
}

float5 operator/(const float5& x, float s)
{
    return float5{
        x[0] / s,
        x[1] / s,
        x[2] / s,
        x[3] / s,
        x[4] / s
    };
}

float6 operator/(float s, const float6& x)
{
    return float6{
        s / x[0],
        s / x[1],
        s / x[2],
        s / x[3],
        s / x[4],
        s / x[5]
    };
}

float6 operator/(const float6& x, float s)
{
    return float6{
        x[0] / s,
        x[1] / s,
        x[2] / s,
        x[3] / s,
        x[4] / s,
        x[5] / s
    };
}

float7 operator/(float s, const float7& x)
{
    return float7{
        s / x[0],
        s / x[1],
        s / x[2],
        s / x[3],
        s / x[4],
        s / x[5],
        s / x[6]
    };
}

float7 operator/(const float7& x, float s)
{
    return float7{
        x[0] / s,
        x[1] / s,
        x[2] / s,
        x[3] / s,
        x[4] / s,
        x[5] / s,
        x[6] / s
    };
}

float2 operator+(const float2& x, const float2& y)
{
    return float2{
        x[0] + y[0],
        x[1] + y[1]
    };
}

float3 operator+(const float3& x, const float3& y)
{
    return float3{
        x[0] + y[0],
        x[1] + y[1],
        x[2] + y[2]
    };
}

float4 operator+(const float4& x, const float4& y)
{
    return float4{
        x[0] + y[0],
        x[1] + y[1],
        x[2] + y[2],
        x[3] + y[3]
    };
}

float5 operator+(const float5& x, const float5& y)
{
    return float5{
        x[0] + y[0],
        x[1] + y[1],
        x[2] + y[2],
        x[3] + y[3],
        x[4] + y[4]
    };
}

float6 operator+(const float6& x, const float6& y)
{
    return float6{
        x[0] + y[0],
        x[1] + y[1],
        x[2] + y[2],
        x[3] + y[3],
        x[4] + y[4],
        x[5] + y[5]
    };
}

float7 operator+(const float7& x, const float7& y)
{
    return float7{
        x[0] + y[0],
        x[1] + y[1],
        x[2] + y[2],
        x[3] + y[3],
        x[4] + y[4],
        x[5] + y[5],
        x[6] + y[6]
    };
}

float2 operator-(const float2& x, const float2& y)
{
    return float2{
        x[0] - y[0],
        x[1] - y[1]
    };
}

float3 operator-(const float3& x, const float3& y)
{
    return float3{
        x[0] - y[0],
        x[1] - y[1],
        x[2] - y[2]
    };
}

float4 operator-(const float4& x, const float4& y)
{
    return float4{
        x[0] - y[0],
        x[1] - y[1],
        x[2] - y[2],
        x[3] - y[3]
    };
}

float5 operator-(const float5& x, const float5& y)
{
    return float5{
        x[0] - y[0],
        x[1] - y[1],
        x[2] - y[2],
        x[3] - y[3],
        x[4] - y[4]
    };
}

float6 operator-(const float6& x, const float6& y)
{
    return float6{
        x[0] - y[0],
        x[1] - y[1],
        x[2] - y[2],
        x[3] - y[3],
        x[4] - y[4],
        x[5] - y[5]
    };
}

float7 operator-(const float7& x, const float7& y)
{
    return float7{
        x[0] - y[0],
        x[1] - y[1],
        x[2] - y[2],
        x[3] - y[3],
        x[4] - y[4],
        x[5] - y[5],
        x[6] - y[6]
    };
}

float2 operator*(const float2& x, const float2& y)
{
    return float2{
        x[0] * y[0],
        x[1] * y[1]
    };
}

float3 operator*(const float3& x, const float3& y)
{
    return float3{
        x[0] * y[0],
        x[1] * y[1],
        x[2] * y[2]
    };
}

float4 operator*(const float4& x, const float4& y)
{
    return float4{
        x[0] * y[0],
        x[1] * y[1],
        x[2] * y[2],
        x[3] * y[3]
    };
}

float5 operator*(const float5& x, const float5& y)
{
    return float5{
        x[0] * y[0],
        x[1] * y[1],
        x[2] * y[2],
        x[3] * y[3],
        x[4] * y[4]
    };
}

float6 operator*(const float6& x, const float6& y)
{
    return float6{
        x[0] * y[0],
        x[1] * y[1],
        x[2] * y[2],
        x[3] * y[3],
        x[4] * y[4],
        x[5] * y[5]
    };
}

float7 operator*(const float7& x, const float7& y)
{
    return float7{
        x[0] * y[0],
        x[1] * y[1],
        x[2] * y[2],
        x[3] * y[3],
        x[4] * y[4],
        x[5] * y[5],
        x[6] * y[6]
    };
}

float2 operator/(const float2& x, const float2& y)
{
    return float2{
        x[0] / y[0],
        x[1] / y[1]
    };
}

float3 operator/(const float3& x, const float3& y)
{
    return float3{
        x[0] / y[0],
        x[1] / y[1],
        x[2] / y[2]
    };
}

float4 operator/(const float4& x, const float4& y)
{
    return float4{
        x[0] / y[0],
        x[1] / y[1],
        x[2] / y[2],
        x[3] / y[3]
    };
}

float5 operator/(const float5& x, const float5& y)
{
    return float5{
        x[0] / y[0],
        x[1] / y[1],
        x[2] / y[2],
        x[3] / y[3],
        x[4] / y[4]
    };
}

float6 operator/(const float6& x, const float6& y)
{
    return float6{
        x[0] / y[0],
        x[1] / y[1],
        x[2] / y[2],
        x[3] / y[3],
        x[4] / y[4],
        x[5] / y[5]
    };
}

float7 operator/(const float7& x, const float7& y)
{
    return float7{
        x[0] / y[0],
        x[1] / y[1],
        x[2] / y[2],
        x[3] / y[3],
        x[4] / y[4],
        x[5] / y[5],
        x[6] / y[6]
    };
}

float2 operator+(const float2& x)
{
    return float2{
        x[0],
        x[1]
    };
}

float3 operator+(const float3& x)
{
    return float3{
        x[0],
        x[1],
        x[2]
    };
}

float4 operator+(const float4& x)
{
    return float4{
        x[0],
        x[1],
        x[2],
        x[3]
    };
}

float5 operator+(const float5& x)
{
    return float5{
        x[0],
        x[1],
        x[2],
        x[3],
        x[4]
    };
}

float6 operator+(const float6& x)
{
    return float6{
        x[0],
        x[1],
        x[2],
        x[3],
        x[4],
        x[5]
    };
}

float7 operator+(const float7& x)
{
    return float7{
        x[0],
        x[1],
        x[2],
        x[3],
        x[4],
        x[5],
        x[6]
    };
}

float2 operator-(const float2& x)
{
    return float2{
        -x[0],
        -x[1]
    };
}

float3 operator-(const float3& x)
{
    return float3{
        -x[0],
        -x[1],
        -x[2]
    };
}

float4 operator-(const float4& x)
{
    return float4{
        -x[0],
        -x[1],
        -x[2],
        -x[3]
    };
}

float5 operator-(const float5& x)
{
    return float5{
        -x[0],
        -x[1],
        -x[2],
        -x[3],
        -x[4]
    };
}

float6 operator-(const float6& x)
{
    return float6{
        -x[0],
        -x[1],
        -x[2],
        -x[3],
        -x[4],
        -x[5]
    };
}

float7 operator-(const float7& x)
{
    return float7{
        -x[0],
        -x[1],
        -x[2],
        -x[3],
        -x[4],
        -x[5],
        -x[6]
    };
}

float dot(const float2& x, const float2& y)
{
    return x[0] * y[0] + x[1] * y[1];
}

float dot(const float3& x, const float3& y)
{
    return x[0] * y[0] + x[1] * y[1] + x[2] * y[2];
}

float dot(const float4& x, const float4& y)
{
    return x[0] * y[0] + x[1] * y[1] + x[2] * y[2] + x[3] * y[3];
}

float dot(const float5& x, const float5& y)
{
    return x[0] * y[0] + x[1] * y[1] + x[2] * y[2] + x[3] * y[3] + x[4] * y[4];
}

float dot(const float6& x, const float6& y)
{
    return x[0] * y[0] + x[1] * y[1] + x[2] * y[2] + x[3] * y[3] + x[4] * y[4] + x[5] * y[5];
}

float dot(const float7& x, const float7& y)
{
    return x[0] * y[0] + x[1] * y[1] + x[2] * y[2] + x[3] * y[3] + x[4] * y[4] + x[5] * y[5] + x[6] * y[6];
}

float length(const float2& x)
{
    return std::sqrt(x[0] * x[0] + x[1] * x[1]);
}

float length(const float3& x)
{
    return std::sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
}

float length(const float4& x)
{
    return std::sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2] + x[3] * x[3]);
}

float length(const float5& x)
{
    return std::sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2] + x[3] * x[3] + x[4] * x[4]);
}

float length(const float6& x)
{
    return std::sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2] + x[3] * x[3] + x[4] * x[4] + x[5] * x[5]);
}

float length(const float7& x)
{
    return std::sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2] + x[3] * x[3] + x[4] * x[4] + x[5] * x[5] + x[6] * x[6]);
}

float distance(const float2& x, const float2& y)
{
    return length(x - y);
}

float distance(const float3& x, const float3& y)
{
    return length(x - y);
}

float distance(const float4& x, const float4& y)
{
    return length(x - y);
}

float distance(const float5& x, const float5& y)
{
    return length(x - y);
}

float distance(const float6& x, const float6& y)
{
    return length(x - y);
}

float distance(const float7& x, const float7& y)
{
    return length(x - y);
}

float3 cross(const float3& a, const float3& b)
{
    return float3{
        a[1] * b[2] - b[1] * a[2],
        a[2] * b[0] - b[2] * a[0],
        a[0] * b[1] - b[0] * a[1]
    };
}

float2 normalize(const float2& x)
{
    return x / length(x);
}

float3 normalize(const float3& x)
{
    return x / length(x);
}

float4 normalize(const float4& x)
{
    return x / length(x);
}

float5 normalize(const float5& x)
{
    return x / length(x);
}

float6 normalize(const float6& x)
{
    return x / length(x);
}

float7 normalize(const float7& x)
{
    return x / length(x);
}
