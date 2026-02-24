#include "frustum.h"

#include <algorithm>
#include <cstdint>

namespace
{

// Extracts row j from a column-major float4x4.
float4 getRow(const float4x4& m, std::uint32_t j)
{
    return { m[0][j], m[1][j], m[2][j], m[3][j] };
}

} // namespace

Frustum::Frustum(const float4x4& view, const float4x4& projection) :
    planes{}
{
    updateViewProjection(*this, view, projection);
}

void updateViewProjection(Frustum& frustum, const float4x4& view, const float4x4& projection)
{
    // clip_pos = projection * view * world_pos
    float4x4 vp = projection * view;

    float4 r0 = getRow(vp, 0);
    float4 r1 = getRow(vp, 1);
    float4 r2 = getRow(vp, 2);
    float4 r3 = getRow(vp, 3);

    // Gribb-Hartmann plane extraction for Vulkan NDC: x,y in [-w,w], z in [0,w]
    frustum.planes[0] = normalize(Plane{ r3.x + r0.x, r3.y + r0.y, r3.z + r0.z, r3.w + r0.w }); // Left
    frustum.planes[1] = normalize(Plane{ r3.x - r0.x, r3.y - r0.y, r3.z - r0.z, r3.w - r0.w }); // Right
    frustum.planes[2] = normalize(Plane{ r3.x + r1.x, r3.y + r1.y, r3.z + r1.z, r3.w + r1.w }); // Bottom
    frustum.planes[3] = normalize(Plane{ r3.x - r1.x, r3.y - r1.y, r3.z - r1.z, r3.w - r1.w }); // Top
    frustum.planes[4] = normalize(Plane{ r2.x, r2.y, r2.z, r2.w });                               // Near (z >= 0)
    frustum.planes[5] = normalize(Plane{ r3.x - r2.x, r3.y - r2.y, r3.z - r2.z, r3.w - r2.w }); // Far  (z <= w)
}

bool isVisible(const Frustum& frustum, const float3& point)
{
    return std::ranges::all_of(frustum.planes, [&](const Plane& plane)
    {
        return signedDistance(plane, point) >= 0.0f;
    });
}

bool isVisible(const Frustum& frustum, const Sphere& sphere)
{
    return std::ranges::all_of(frustum.planes, [&](const Plane& plane)
    {
        return signedDistance(plane, sphere.center) + sphere.radius >= 0.0f;
    });
}

bool isVisible(const Frustum& frustum, const AABB& aabb)
{
    // P-vertex method: for each plane, test the corner most aligned with the normal.
    // If even the most positive corner is behind the plane, the AABB is outside.
    for (const auto& plane : frustum.planes)
    {
        float3 p;
        p.x = plane.x >= 0.0f ? aabb.max_point.x : aabb.min_point.x;
        p.y = plane.y >= 0.0f ? aabb.max_point.y : aabb.min_point.y;
        p.z = plane.z >= 0.0f ? aabb.max_point.z : aabb.min_point.z;

        if (signedDistance(plane, p) < 0.0f)
        {
            return false;
        }
    }
    return true;
}
