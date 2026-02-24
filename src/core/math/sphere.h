#ifndef CORE_MATH_SPHERE_H_
#define CORE_MATH_SPHERE_H_

#include "vector.h"

// Geometric sphere defined by a center point and radius.
struct Sphere
{
    float3 center{};
    float radius{ 0.0f };

    Sphere() = default;

    Sphere(const float3& center, float radius);
};

// Signed distance from the sphere surface to a point.
// Negative: point is inside the sphere.
float signedDistance(const Sphere& sphere, const float3& point);

// Returns true if the point is inside or on the sphere surface.
bool contains(const Sphere& sphere, const float3& point);

#endif /* CORE_MATH_SPHERE_H_ */
