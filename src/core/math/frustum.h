#ifndef CORE_MATH_FRUSTUM_H_
#define CORE_MATH_FRUSTUM_H_

#include "aabb.h"
#include "matrix.h"
#include "plane.h"
#include "sphere.h"

// View frustum defined by six planes extracted from a view-projection matrix.
// Plane order: left, right, bottom, top, near, far.
// Assumes Vulkan NDC: x in [-1, 1], y in [-1, 1], z in [0, 1].
struct Frustum
{
    Plane planes[6]{};

    Frustum() = default;

    Frustum(const float4x4& view, const float4x4& projection);
};

// Recomputes the six frustum planes from updated view and projection matrices.
void updateViewProjection(Frustum& frustum, const float4x4& view, const float4x4& projection);

// Returns true if the point is inside or on the frustum boundary.
bool isVisible(const Frustum& frustum, const float3& point);

// Returns true if the sphere intersects or is inside the frustum.
bool isVisible(const Frustum& frustum, const Sphere& sphere);

// Returns true if the AABB intersects or is inside the frustum.
// Uses the p-vertex method for efficient per-plane testing.
bool isVisible(const Frustum& frustum, const AABB& aabb);

#endif /* CORE_MATH_FRUSTUM_H_ */
