#ifndef CORE_MATH_QUATERNION_H_
#define CORE_MATH_QUATERNION_H_

#include <cstdint>

struct float4x4;

struct quaternion
{
    float x{ 0.0f };
    float y{ 0.0f };
    float z{ 0.0f };
    float w{ 1.0f };

    const float& operator[](std::uint32_t index) const;

    float& operator[](std::uint32_t index);

    operator float4x4() const;

    quaternion() = default;

    quaternion(float x, float y, float z, float w);

    quaternion(const quaternion& other);
};

quaternion operator*(float s, const quaternion& q);

quaternion operator*(const quaternion& q, float s);

quaternion operator*(const quaternion& x, const quaternion& y);

quaternion operator/(float s, const quaternion& q);

quaternion operator/(const quaternion& q, float s);

quaternion operator+(const quaternion& x, const quaternion& y);

quaternion operator-(const quaternion& x, const quaternion& y);

float norm(const quaternion& q);

quaternion normalize(const quaternion& q);

quaternion conjugate(const quaternion& q);

quaternion inverse(const quaternion& q);

#endif /* CORE_MATH_QUATERNION_H_ */
