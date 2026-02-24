// math.slang - Math utility module
// Common mathematical operations and matrix utilities

module math;

public static const float PI         = 3.14159265359;
public static const float TWO_PI     = 6.28318530718;
public static const float INV_PI     = 0.31830988618;
public static const float INV_TWO_PI = 0.15915494309;

// Matrix multiplication operators
public float4x4 operator*(float4x4 a, float4x4 b)
{
    return mul(a, b);
}

public float4 operator*(float4x4 a, float4 v)
{
    return mul(a, v);
}

public float3 operator*(float3x3 a, float3 v)
{
    return mul(a, v);
}

public float3x3 operator*(float3x3 a, float3x3 b)
{
    return mul(a, b);
}

public float2 operator *(float2x2 a, float2 v)
{
    return mul(a, v);
}

public float2x2 operator *(float2x2 a, float2x2 b)
{
    return mul(a, b);
}

// ============================================================================
// Spherical coordinates — Y-up convention
// theta: azimuth around Y axis in XZ plane [0, 2*PI]
// phi:   polar angle from +Y axis          [0, PI]
// r:     radius                            [0, inf)
// ============================================================================

public struct SphericalCoordinate
{
    public float theta;
    public float phi;
    public float r;
};

// Converts a Cartesian direction/position to spherical coordinates.
public SphericalCoordinate toSpherical(float3 cartesian)
{
    SphericalCoordinate s;
    s.r = length(cartesian);
    if (s.r == 0.0)
    {
        s.theta = 0.0;
        s.phi = 0.0;
        return s;
    }
    s.phi = acos(clamp(cartesian.y / s.r, -1.0, 1.0));
    s.theta = atan2(cartesian.z, cartesian.x);
    if (s.theta < 0.0)
    {
        s.theta += TWO_PI;
    }
    return s;
}

// Converts spherical coordinates to a Cartesian position.
public float3 toCartesian(SphericalCoordinate s)
{
    float sin_phi = sin(s.phi);
    return float3(s.r * sin_phi * cos(s.theta), s.r * cos(s.phi), s.r * sin_phi * sin(s.theta));
}

// Projects spherical coordinates onto the unit sphere (r = 1).
public SphericalCoordinate normalize(SphericalCoordinate s)
{
    SphericalCoordinate result;
    result.theta = s.theta;
    result.phi = s.phi;
    result.r = 1.0;
    return result;
}

// Converts a unit quaternion (x, y, z, w) to a 3x3 rotation matrix
public float3x3 quaternionToMatrix(float4 q)
{
    float xx = q.x * q.x;
    float yy = q.y * q.y;
    float zz = q.z * q.z;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float yz = q.y * q.z;
    float wx = q.w * q.x;
    float wy = q.w * q.y;
    float wz = q.w * q.z;

    return float3x3(
        1.0 - 2.0 * (yy + zz),  2.0 * (xy - wz),         2.0 * (xz + wy),
        2.0 * (xy + wz),         1.0 - 2.0 * (xx + zz),  2.0 * (yz - wx),
        2.0 * (xz - wy),         2.0 * (yz + wx),         1.0 - 2.0 * (xx + yy)
    );
}
