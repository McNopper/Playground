#ifndef CORE_MATH_AABB_H_
#define CORE_MATH_AABB_H_

#include "vector.h"

// Axis-aligned bounding box defined by minimum and maximum corner points.
struct AABB
{
    float3 min_point{};
    float3 max_point{};

    AABB() = default;

    AABB(const float3& min_point, const float3& max_point);
};

// Creates an AABB from a center point and half-extents.
AABB createAABB(const float3& center, const float3& half_extents);

// Returns the center of the AABB.
float3 center(const AABB& aabb);

// Returns the half-extents (half the size in each axis) of the AABB.
float3 halfExtents(const AABB& aabb);

// Returns true if the point is inside or on the surface of the AABB.
bool contains(const AABB& aabb, const float3& point);

// Returns the smallest AABB that contains both input AABBs.
AABB merge(const AABB& a, const AABB& b);

#endif /* CORE_MATH_AABB_H_ */
