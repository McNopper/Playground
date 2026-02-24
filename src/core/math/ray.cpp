#include "ray.h"

#include <algorithm>
#include <cmath>
#include <limits>

Ray::Ray(const float3& origin, const float3& direction) :
    origin{ origin }, direction{ direction }
{
}

std::optional<float> intersect(const Ray& ray, const Plane& plane)
{
    float3 normal{ plane.x, plane.y, plane.z };
    float denom{ dot(ray.direction, normal) };

    if (std::abs(denom) < 1e-6f)
    {
        return std::nullopt;
    }

    float t{ -(dot(ray.origin, normal) + plane.w) / denom };
    if (t < 0.0f)
    {
        return std::nullopt;
    }

    return t;
}

std::optional<float> intersect(const Ray& ray, const Sphere& sphere)
{
    float3 oc{ ray.origin - sphere.center };
    float b{ dot(oc, ray.direction) };
    float c{ dot(oc, oc) - sphere.radius * sphere.radius };
    float discriminant{ b * b - c };

    if (discriminant < 0.0f)
    {
        return std::nullopt;
    }

    float sqrt_disc{ std::sqrt(discriminant) };
    float t{ -b - sqrt_disc };
    if (t >= 0.0f)
    {
        return t;
    }

    t = -b + sqrt_disc;
    if (t >= 0.0f)
    {
        return t;
    }

    return std::nullopt;
}

std::optional<float> intersect(const Ray& ray, const AABB& aabb)
{
    float t_min{ 0.0f };
    float t_max{ std::numeric_limits<float>::max() };

    for (int i{ 0 }; i < 3; ++i)
    {
        float origin_i{ (&ray.origin.x)[i] };
        float dir_i{ (&ray.direction.x)[i] };
        float min_i{ (&aabb.min_point.x)[i] };
        float max_i{ (&aabb.max_point.x)[i] };

        if (std::abs(dir_i) < 1e-8f)
        {
            if (origin_i < min_i || origin_i > max_i)
            {
                return std::nullopt;
            }
        }
        else
        {
            float inv{ 1.0f / dir_i };
            float t0{ (min_i - origin_i) * inv };
            float t1{ (max_i - origin_i) * inv };
            if (t0 > t1)
            {
                std::swap(t0, t1);
            }
            t_min = std::max(t_min, t0);
            t_max = std::min(t_max, t1);
            if (t_min > t_max)
            {
                return std::nullopt;
            }
        }
    }

    return t_min;
}

std::optional<float> intersect(const Ray& ray, const float3& v0, const float3& v1, const float3& v2)
{
    constexpr float epsilon{ 1e-6f };

    float3 edge1{ v1 - v0 };
    float3 edge2{ v2 - v0 };
    float3 h{ cross(ray.direction, edge2) };
    float a{ dot(edge1, h) };

    if (std::abs(a) < epsilon)
    {
        return std::nullopt;
    }

    float inv_a{ 1.0f / a };
    float3 s{ ray.origin - v0 };
    float u{ inv_a * dot(s, h) };

    if (u < 0.0f || u > 1.0f)
    {
        return std::nullopt;
    }

    float3 q{ cross(s, edge1) };
    float v{ inv_a * dot(ray.direction, q) };

    if (v < 0.0f || u + v > 1.0f)
    {
        return std::nullopt;
    }

    float t{ inv_a * dot(edge2, q) };
    if (t < 0.0f)
    {
        return std::nullopt;
    }

    return t;
}
