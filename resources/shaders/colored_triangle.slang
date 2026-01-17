// Colored triangle shader
// Demonstrates per-vertex colors with transformation matrices

import math;

[[vk::binding(0, 0)]]
cbuffer UniformViewData
{
    float4x4 u_projectionMatrix;
    float4x4 u_viewMatrix;
}

[[vk::binding(1, 0)]]
cbuffer UniformModelData
{
    float4x4 u_worldMatrix;
}

struct VertexInputData
{
    float3	i_position  : POSITION;
    float4	i_color_0   : COLOR0;
};

struct VertexVaryingData
{
    float4 position     : SV_Position;
    float4 color_0      : COLOR0;
};

[shader("vertex")]
VertexVaryingData vertexMain(VertexInputData input)
{
    VertexVaryingData output;

    output.color_0 = input.i_color_0;
    output.position = u_projectionMatrix * u_viewMatrix * u_worldMatrix * float4(input.i_position, 1.0);

    return output;
}

[shader("fragment")]
float4 fragmentMain(VertexVaryingData input) : SV_Target
{
    return input.color_0;
}
