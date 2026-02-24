#include "plane.h"

#include <cmath>

Plane::Plane(float x, float y, float z, float w) :
    x{ x }, y{ y }, z{ z }, w{ w }
{
}

Plane::Plane(const float3& normal, float d) :
    x{ normal.x }, y{ normal.y }, z{ normal.z }, w{ d }
{
}

Plane createPlane(const float3& point, const float3& normal)
{
    float3 n = normalize(normal);
    float d = -(n.x * point.x + n.y * point.y + n.z * point.z);
    return { n.x, n.y, n.z, d };
}

float signedDistance(const Plane& plane, const float3& point)
{
    return plane.x * point.x + plane.y * point.y + plane.z * point.z + plane.w;
}

Plane normalize(const Plane& plane)
{
    float len = std::sqrt(plane.x * plane.x + plane.y * plane.y + plane.z * plane.z);
    return { plane.x / len, plane.y / len, plane.z / len, plane.w / len };
}
