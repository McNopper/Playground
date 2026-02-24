// brdf_lut.slang - BRDF Integration Map Generator
// Generates a 2D lookup texture for split-sum approximation in PBR
// Based on Epic Games' approach for Image-Based Lighting (IBL)
//
// Output: RG texture where:
// - R: Scale factor for F0 (Fresnel at 0 degrees)
// - G: Bias factor for F0
// - X-axis: NdotV (0 to 1)
// - Y-axis: Roughness (0 to 1)

import math;
import pbr;
import sampling;

RWTexture2D<float4> output_texture;

// Integrate BRDF for a single NdotV and roughness value
float2 integrateBRDF(float NdotV, float roughness)
{
    float3 V;
    V.x = sqrt(1.0 - NdotV * NdotV);
    V.y = 0.0;
    V.z = NdotV;
    
    float A = 0.0;
    float B = 0.0;
    
    float3 N = float3(0.0, 0.0, 1.0);
    
    const uint SAMPLE_COUNT = 1024u;
    for (uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        float2 Xi = hammersley(i, SAMPLE_COUNT);
        float3 H = importanceSampleGGX(Xi, N, roughness);
        float3 L = normalize(2.0 * dot(V, H) * H - V);
        
        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);
        
        if (NdotL > 0.0)
        {
            float G = geometrySmith(N, V, L, roughness);
            float G_Vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);
            
            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    
    A /= float(SAMPLE_COUNT);
    B /= float(SAMPLE_COUNT);
    
    return float2(A, B);
}

[shader("compute")]
[numthreads(8, 8, 1)]
void computeMain(uint3 threadId : SV_DispatchThreadID)
{
    uint2 dimensions;
    output_texture.GetDimensions(dimensions.x, dimensions.y);
    
    if (threadId.x >= dimensions.x || threadId.y >= dimensions.y)
        return;
    
    // Map pixel coordinates to [0, 1] range
    // X-axis: NdotV (0 to 1, left to right)
    // Y-axis: Roughness (0 to 1, bottom to top)
    float NdotV = (float(threadId.x) + 0.5) / float(dimensions.x);
    float roughness = (float(dimensions.y - threadId.y - 1) + 0.5) / float(dimensions.y); // Flip Y for bottom-to-top
    
    // Ensure NdotV is never exactly 0
    NdotV = max(NdotV, 0.001);
    
    float2 result = integrateBRDF(NdotV, roughness);
    
    output_texture[threadId.xy] = float4(result.x, result.y, 0.0, 1.0);
}
