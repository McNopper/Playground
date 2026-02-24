#include "spherical_coordinate.h"

#include <algorithm>
#include <cmath>

SphericalCoordinate toSpherical(const float3& cartesian)
{
    SphericalCoordinate s{};
    s.r = std::sqrt(cartesian.x * cartesian.x + cartesian.y * cartesian.y + cartesian.z * cartesian.z);
    if (s.r == 0.0f)
    {
        return s;
    }
    s.phi = std::acos(std::clamp(cartesian.y / s.r, -1.0f, 1.0f));
    s.theta = std::atan2(cartesian.z, cartesian.x);
    if (s.theta < 0.0f)
    {
        s.theta += 2.0f * 3.14159265359f;
    }
    return s;
}

float3 toCartesian(const SphericalCoordinate& s)
{
    float sin_phi{ std::sin(s.phi) };
    return {
        s.r * sin_phi * std::cos(s.theta),
        s.r * std::cos(s.phi),
        s.r * sin_phi * std::sin(s.theta)
    };
}

SphericalCoordinate normalize(const SphericalCoordinate& s)
{
    return SphericalCoordinate{ s.theta, s.phi, 1.0f };
}
