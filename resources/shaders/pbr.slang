// pbr.slang - Physically Based Rendering math module
// Core BRDF functions for the glTF 2.0 metallic-roughness model

module pbr;

import math;

// ============================================================================
// Fresnel
// ============================================================================

// Schlick approximation of the Fresnel term
public float3 fresnelSchlick(float cos_theta, float3 f0)
{
    return f0 + (1.0 - f0) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}

// Fresnel with roughness bias for IBL ambient specular
public float3 fresnelSchlickRoughness(float cos_theta, float3 f0, float roughness)
{
    return f0 + (max(1.0 - roughness, f0) - f0) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}

// ============================================================================
// Normal Distribution Function
// ============================================================================

// GGX / Trowbridge-Reitz NDF
public float distributionGGX(float3 n, float3 h, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float n_dot_h = max(dot(n, h), 0.0);
    float n_dot_h2 = n_dot_h * n_dot_h;

    float denom = n_dot_h2 * (a2 - 1.0) + 1.0;
    denom = PI * denom * denom;

    return a2 / denom;
}

// ============================================================================
// Geometry / Visibility
// ============================================================================

// Schlick-GGX geometry term (single direction)
public float geometrySchlickGGX(float n_dot_v, float roughness)
{
    float a = roughness;
    float k = (a * a) / 2.0;

    float denom = n_dot_v * (1.0 - k) + k;

    return n_dot_v / denom;
}

// Smith's method combining geometry terms for both light and view directions
public float geometrySmith(float3 n, float3 v, float3 l, float roughness)
{
    float n_dot_v = max(dot(n, v), 0.0);
    float n_dot_l = max(dot(n, l), 0.0);
    float ggx2 = geometrySchlickGGX(n_dot_v, roughness);
    float ggx1 = geometrySchlickGGX(n_dot_l, roughness);

    return ggx1 * ggx2;
}

// ============================================================================
// Material helpers
// ============================================================================

// glTF 2.0 metallic-roughness F0: dielectric base (0.04) blended to albedo by metallic
public float3 f0FromMetallic(float3 albedo, float metallic)
{
    return lerp(float3(0.04), albedo, metallic);
}

// ============================================================================
// Specular BRDF
// ============================================================================

// Cook-Torrance specular BRDF: D*G*F / (4 * NdotV * NdotL)
public float3 cookTorranceSpecular(float3 n, float3 v, float3 l, float roughness, float3 f0)
{
    float3 h = normalize(v + l);
    float n_dot_v = max(dot(n, v), 0.0);
    float n_dot_l = max(dot(n, l), 0.0);
    float v_dot_h = max(dot(v, h), 0.0);

    float d = distributionGGX(n, h, roughness);
    float g = geometrySmith(n, v, l, roughness);
    float3 f = fresnelSchlick(v_dot_h, f0);

    float3 numerator = d * g * f;
    float denominator = max(4.0 * n_dot_v * n_dot_l, 0.0001);

    return numerator / denominator;
}

// ============================================================================
// Diffuse
// ============================================================================

// Lambertian diffuse BRDF
public float3 lambertianDiffuse(float3 albedo)
{
    return albedo * INV_PI;
}

// ============================================================================
// IBL Sampling
// ============================================================================

// Samples a preconvolved diffuse irradiance cube map
public float3 sampleIBLDiffuse(TextureCube<float4> irradiance_map, SamplerState sampler_state, float3 normal)
{
    return irradiance_map.Sample(sampler_state, normal).rgb;
}

// Samples a prefiltered specular environment cube map at the roughness mip level
public float3 sampleIBLSpecular(TextureCube<float4> prefiltered_map, SamplerState sampler_state, float3 r, float roughness, float mip_count)
{
    float mip_level = roughness * (mip_count - 1.0);
    return prefiltered_map.SampleLevel(sampler_state, r, mip_level).rgb;
}

// Samples the BRDF integration LUT for the split-sum ambient specular
public float2 sampleBRDFLUT(Texture2D<float4> brdf_lut, SamplerState sampler_state, float n_dot_v, float roughness)
{
    return brdf_lut.Sample(sampler_state, float2(n_dot_v, roughness)).rg;
}

// Van der Corput radical inverse for Hammersley sequence
public float radicalInverseVdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

// Hammersley low-discrepancy sequence point i of N
public float2 hammersley(uint i, uint n)
{
    return float2(float(i) / float(n), radicalInverseVdC(i));
}

// GGX importance sampling — returns a half-vector in world space
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
