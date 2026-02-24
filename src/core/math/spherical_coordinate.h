#ifndef CORE_MATH_SPHERICAL_COORDINATE_H_
#define CORE_MATH_SPHERICAL_COORDINATE_H_

#include "vector.h"

// Spherical coordinate in Y-up convention:
//   theta - azimuth angle around the Y axis in the XZ plane [0, 2*PI]
//   phi   - polar angle from the +Y axis                    [0, PI]
//   r     - radius (distance from origin)                   [0, inf)
//
// Cartesian mapping:
//   x = r * sin(phi) * cos(theta)
//   y = r * cos(phi)
//   z = r * sin(phi) * sin(theta)
struct SphericalCoordinate
{
    float theta{ 0.0f };
    float phi{ 0.0f };
    float r{ 1.0f };
};

// Converts a Cartesian (x, y, z) direction/position to spherical coordinates.
SphericalCoordinate toSpherical(const float3& cartesian);

// Converts spherical coordinates to a Cartesian (x, y, z) position.
float3 toCartesian(const SphericalCoordinate& s);

// Returns the spherical coordinate projected onto the unit sphere (r = 1).
SphericalCoordinate normalize(const SphericalCoordinate& s);

#endif /* CORE_MATH_SPHERICAL_COORDINATE_H_ */
