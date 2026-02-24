#ifndef CORE_MATH_RAY_H_
#define CORE_MATH_RAY_H_

#include "aabb.h"
#include "plane.h"
#include "sphere.h"
#include "vector.h"

#include <optional>

// A ray defined by an origin point and a normalized direction.
struct Ray
{
    float3 origin{};
    float3 direction{ 0.0f, 0.0f, -1.0f };

    Ray() = default;

    Ray(const float3& origin, const float3& direction);
};

// Returns the t parameter (distance along the ray) at the hit point, or
// std::nullopt if there is no intersection. t >= 0 means the hit is in front
// of the origin. Hit position = origin + t * direction.

// Ray vs infinite plane.
std::optional<float> intersect(const Ray& ray, const Plane& plane);

// Ray vs sphere. Returns the nearest t >= 0 (entry point).
std::optional<float> intersect(const Ray& ray, const Sphere& sphere);

// Ray vs axis-aligned bounding box (slab method). Returns t at entry face.
std::optional<float> intersect(const Ray& ray, const AABB& aabb);

// Ray vs triangle (Möller–Trumbore). Returns t >= 0 for a front-face hit.
// v0, v1, v2 are the triangle vertices in counter-clockwise winding order.
std::optional<float> intersect(const Ray& ray, const float3& v0, const float3& v1, const float3& v2);

#endif /* CORE_MATH_RAY_H_ */
