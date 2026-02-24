#ifndef CORE_MATH_PLANE_H_
#define CORE_MATH_PLANE_H_

#include "vector.h"

// Geometric plane: ax + by + cz + d = 0
// (x, y, z) = unit normal pointing to the positive (front) side, w = d
struct Plane
{
    float x{ 0.0f };
    float y{ 0.0f };
    float z{ 0.0f };
    float w{ 0.0f };

    Plane() = default;

    Plane(float x, float y, float z, float w);

    Plane(const float3& normal, float d);
};

// Creates a plane from a point on the plane and an outward-facing normal.
Plane createPlane(const float3& point, const float3& normal);

// Signed distance from the plane to a point.
// Assumes the plane has a unit-length normal.
// Positive: point is on the side the normal points to (front).
// Negative: point is on the opposite side (back).
float signedDistance(const Plane& plane, const float3& point);

// Returns a plane with a unit-length normal.
Plane normalize(const Plane& plane);

#endif /* CORE_MATH_PLANE_H_ */
