// Textured quad shader
// Demonstrates basic texturing with proper color space handling

import math;
import colorspace;

[[vk::binding(0, 0)]]
cbuffer UniformViewData
{
    float4x4 u_projection_matrix;
    float4x4 u_view_matrix;
}

[[vk::binding(1, 0)]]
cbuffer UniformModelData
{
    float4x4 u_world_matrix;
}

[[vk::binding(0, 1)]]
Texture2D u_texture;

[[vk::binding(1, 1)]]
SamplerState u_sampler;

struct VertexInputData
{
    float3	i_position  : POSITION;
    float2	i_texcoord  : TEXCOORD0;
};

struct VertexVaryingData
{
    float4 position     : SV_Position;
    float2 texcoord     : TEXCOORD0;
};

[shader("vertex")]
VertexVaryingData vertexMain(VertexInputData input)
{
    VertexVaryingData output;

    output.texcoord = input.i_texcoord;
    output.position = u_projection_matrix * u_view_matrix * u_world_matrix * float4(input.i_position, 1.0);

    return output;
}

[shader("fragment")]
float4 fragmentMain(VertexVaryingData input) : SV_Target
{
    // Color pipeline:
    // 1. Sample texture: sRGB texture → GPU auto-converts to linear
    // 2. Shader processing: Work in linear color space (scene-referred)
    // 3. Output encoding: Convert to display-referred encoding
    //
    // Current: SDR output (sRGB encoding to UNORM framebuffer)
    // Future: Could be PQ/HLG for HDR, or linear for FP16 render targets
    
    float4 color = u_texture.Sample(u_sampler, input.texcoord);
    
    // Apply output encoding based on target format
    color.rgb = linearToSRGB(color.rgb);  // SDR: sRGB encoding
    // color.rgb = linearToPQ(color.rgb);     // HDR: PQ encoding (HDR10)
    // color.rgb = linearToHLG(color.rgb);    // HDR: HLG encoding
    // color.rgb stays linear for FP16/scRGB render targets
    
    return color;
}
