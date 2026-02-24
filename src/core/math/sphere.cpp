#include "sphere.h"

Sphere::Sphere(const float3& center, float radius) :
    center{ center }, radius{ radius }
{
}

float signedDistance(const Sphere& sphere, const float3& point)
{
    return length(point - sphere.center) - sphere.radius;
}

bool contains(const Sphere& sphere, const float3& point)
{
    return signedDistance(sphere, point) <= 0.0f;
}
