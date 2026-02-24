// sampling.slang - Sampling utility module
// Low-discrepancy sequences and hemisphere sampling for Monte Carlo integration

module sampling;

import math;

// ============================================================================
// Low-discrepancy sequences
// ============================================================================

// Van der Corput radical inverse — base-2 component of the Hammersley sequence
public float radicalInverseVdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

// Hammersley 2D low-discrepancy sequence — point i of n
// xi.x is stratified in [0,1), xi.y is the Van der Corput radical inverse
public float2 hammersley(uint i, uint n)
{
    return float2(float(i) / float(n), radicalInverseVdC(i));
}

// Halton sequence — base-b radical inverse
// Use base 2 and 3 for a 2D sequence: float2(halton(i, 2), halton(i, 3))
public float halton(uint i, uint base)
{
    float result = 0.0;
    float inv_base = 1.0 / float(base);
    float fraction = inv_base;
    uint index = i;
    while (index > 0u)
    {
        result += float(index % base) * fraction;
        index /= base;
        fraction *= inv_base;
    }
    return result;
}

// ============================================================================
// Hemisphere sampling — all functions return a direction in tangent space
// (+Z is the hemisphere axis). Transform to world space with importanceSampleGGX
// or manually via a TBN matrix.
// ============================================================================

// Uniform hemisphere sampling — PDF = 1 / (2*PI)
// Use for ambient occlusion and unweighted Monte Carlo integration
public float3 uniformHemisphere(float2 xi)
{
    float sin_theta = sqrt(1.0 - xi.x * xi.x);
    float phi = TWO_PI * xi.y;
    return float3(cos(phi) * sin_theta, sin(phi) * sin_theta, xi.x);
}

// Cosine-weighted hemisphere sampling — PDF = cos(theta) / PI
// Optimal for Lambertian/diffuse surfaces; reduces variance vs uniform sampling
public float3 cosineHemisphere(float2 xi)
{
    // Malley's method via concentric disk projection
    float r = sqrt(xi.x);
    float phi = TWO_PI * xi.y;
    float x = r * cos(phi);
    float y = r * sin(phi);
    float z = sqrt(max(0.0, 1.0 - xi.x));
    return float3(x, y, z);
}

// GGX importance sampling — PDF = D(h) * cos(theta_h)
// Returns a microfacet half-vector in world space aligned to normal n
public float3 importanceSampleGGX(float2 xi, float3 n, float roughness)
{
    float a = roughness * roughness;

    float phi = TWO_PI * xi.x;
    float cos_theta = sqrt((1.0 - xi.y) / (1.0 + (a * a - 1.0) * xi.y));
    float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

    float3 h;
    h.x = cos(phi) * sin_theta;
    h.y = sin(phi) * sin_theta;
    h.z = cos_theta;

    // Tangent space to world space
    float3 up = abs(n.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    float3 tangent = normalize(cross(up, n));
    float3 bitangent = cross(n, tangent);

    float3 sample_vec = tangent * h.x + bitangent * h.y + n * h.z;
    return normalize(sample_vec);
}

// ============================================================================
// Disk sampling
// ============================================================================

// Concentric disk mapping (Shirley) — uniform area distribution on unit disk
// Use for soft shadows, depth-of-field lens sampling
public float2 concentricDisk(float2 xi)
{
    float2 offset = 2.0 * xi - float2(1.0, 1.0);
    if (offset.x == 0.0 && offset.y == 0.0)
    {
        return float2(0.0, 0.0);
    }
    float r;
    float phi;
    if (abs(offset.x) > abs(offset.y))
    {
        r = offset.x;
        phi = (PI / 4.0) * (offset.y / offset.x);
    }
    else
    {
        r = offset.y;
        phi = (PI / 2.0) - (PI / 4.0) * (offset.x / offset.y);
    }
    return float2(r * cos(phi), r * sin(phi));
}
