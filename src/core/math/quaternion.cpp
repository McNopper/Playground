#include "quaternion.h"

#include <cmath>

#include "matrix.h"

const float& quaternion::operator[](std::uint32_t index) const
{
    return *(reinterpret_cast<const float*>(this) + index);
}

float& quaternion::operator[](std::uint32_t index)
{
    return *(reinterpret_cast<float*>(this) + index);
}

quaternion::operator float4x4() const
{
    return {
        {
            1.0f - 2.0f * y * y - 2.0f * z * z,
            2.0f * x * y + 2.0f * w * z,
            2.0f * x * z - 2.0f * w * y,
            0.0f
        },
        {
            2.0f * x * y - 2.0f * w * z,
            1.0f - 2.0f * x * x - 2.0f * z * z,
            2.0f * y * z + 2.0f * w * x,
            0.0f
        },
        {
            2.0f * x * z + 2.0f * w * y,
            2.0f * y * z - 2.0f * w * x,
            1.0f - 2.0f * x * x - 2.0f * y * y,
            0.0f
        },
        {
            0.0f,
            0.0f,
            0.0f,
            1.0f
        }
    };
}

quaternion::quaternion(float x, float y, float z, float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

quaternion::quaternion(const quaternion& other)
{
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;
}

quaternion operator*(float s, const quaternion& q)
{
    return {
        s * q[0],
        s * q[1],
        s * q[2],
        s * q[3]
    };
}

quaternion operator*(const quaternion& q, float s)
{
    return {
        q[0] * s,
        q[1] * s,
        q[2] * s,
        q[3] * s
    };
}

quaternion operator*(const quaternion& x, const quaternion& y)
{
    return {
        x[3] * y[0] + x[0] * y[3] + x[1] * y[2] - x[2] * y[1],
        x[3] * y[1] - x[0] * y[2] + x[1] * y[3] + x[2] * y[0],
        x[3] * y[2] + x[0] * y[1] - x[1] * y[0] + x[2] * y[3],
        x[3] * y[3] - x[0] * y[0] - x[1] * y[1] - x[2] * y[2]
    };
};

quaternion operator/(float s, const quaternion& q)
{
    return {
        s / q[0],
        s / q[1],
        s / q[2],
        s / q[3]
    };
}

quaternion operator/(const quaternion& q, float s)
{
    return {
        q[0] / s,
        q[1] / s,
        q[2] / s,
        q[3] / s
    };
}

quaternion operator+(const quaternion& x, const quaternion& y)
{
    return {
        x[0] + y[0],
        x[1] + y[1],
        x[2] + y[2],
        x[3] + y[3]
    };
}

quaternion operator-(const quaternion& x, const quaternion& y)
{
    return {
        x[0] - y[0],
        x[1] - y[1],
        x[2] - y[2],
        x[3] - y[3]
    };
}

float norm(const quaternion& q)
{
    return std::sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
}

quaternion normalize(const quaternion& q)
{
    return q / norm(q);
}

quaternion conjugate(const quaternion& q)
{
    return {
        -q[0],
        -q[1],
        -q[2],
        q[3],
    };
}

quaternion inverse(const quaternion& q)
{
    float n = norm(q);

    return conjugate(q) / (n * n);
}
