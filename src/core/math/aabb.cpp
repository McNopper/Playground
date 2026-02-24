#include "aabb.h"

#include <algorithm>

AABB::AABB(const float3& min_point, const float3& max_point) :
    min_point{ min_point }, max_point{ max_point }
{
}

AABB createAABB(const float3& center, const float3& half_extents)
{
    return { center - half_extents, center + half_extents };
}

float3 center(const AABB& aabb)
{
    return (aabb.min_point + aabb.max_point) * 0.5f;
}

float3 halfExtents(const AABB& aabb)
{
    return (aabb.max_point - aabb.min_point) * 0.5f;
}

bool contains(const AABB& aabb, const float3& point)
{
    return point.x >= aabb.min_point.x && point.x <= aabb.max_point.x &&
           point.y >= aabb.min_point.y && point.y <= aabb.max_point.y &&
           point.z >= aabb.min_point.z && point.z <= aabb.max_point.z;
}

AABB merge(const AABB& a, const AABB& b)
{
    return {
        { std::min(a.min_point.x, b.min_point.x),
          std::min(a.min_point.y, b.min_point.y),
          std::min(a.min_point.z, b.min_point.z) },
        { std::max(a.max_point.x, b.max_point.x),
          std::max(a.max_point.y, b.max_point.y),
          std::max(a.max_point.z, b.max_point.z) }
    };
}
