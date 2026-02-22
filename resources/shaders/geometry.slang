// geometry.slang - Surface geometry utilities
// Tangent space construction, normal map decoding, and direction mapping

module geometry;

import math;

// Decodes a tangent-space normal from a normal map texel [0, 1] to [-1, 1]
public float3 unpackNormalMap(float3 sample)
{
    return normalize(sample * 2.0 - 1.0);
}

// Builds a tangent-to-world TBN matrix from interpolated vertex attributes.
// normal and tangent must be in world space and pre-normalized.
// tangent.w encodes the handedness of the bitangent (+1 or -1).
public float3x3 buildTBN(float3 normal, float4 tangent)
{
    float3 n = normalize(normal);
    float3 t = normalize(tangent.xyz);
    float3 b = cross(n, t) * tangent.w;

    return float3x3(
        t.x, b.x, n.x,
        t.y, b.y, n.y,
        t.z, b.z, n.z
    );
}

// Transforms a tangent-space normal to world space using a TBN matrix
public float3 applyTBN(float3x3 tbn, float3 tangent_normal)
{
    return normalize(mul(tbn, tangent_normal));
}

// ============================================================================
// Environment mapping
// ============================================================================

// Converts a world direction to equirectangular (lat-long) UV coordinates
public float2 directionToEquirectangularUV(float3 dir)
{
    float u = 0.5 + atan2(dir.z, dir.x) * INV_TWO_PI;
    float v = 0.5 - asin(clamp(dir.y, -1.0, 1.0)) * INV_PI;
    return float2(u, v);
}

// Converts a cube map face index and UV coordinates [0, 1] to a world direction.
// Face indices follow the Vulkan cube map convention:
// 0=+X, 1=-X, 2=+Y, 3=-Y, 4=+Z, 5=-Z
public float3 cubeFaceUVToDirection(uint face, float2 uv)
{
    float s = uv.x * 2.0 - 1.0;
    float t = uv.y * 2.0 - 1.0;
    switch (face)
    {
        case 0u: return normalize(float3( 1.0, -t, -s)); // +X
        case 1u: return normalize(float3(-1.0, -t,  s)); // -X
        case 2u: return normalize(float3(   s,  1.0,  t)); // +Y
        case 3u: return normalize(float3(   s, -1.0, -t)); // -Y
        case 4u: return normalize(float3(   s, -t,  1.0)); // +Z
        default: return normalize(float3(  -s, -t, -1.0)); // -Z
    }
}
