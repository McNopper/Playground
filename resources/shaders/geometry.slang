// geometry.slang - Surface geometry utilities
// Tangent space construction and normal map decoding

module geometry;

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
