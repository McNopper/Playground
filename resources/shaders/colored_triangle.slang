// Colored triangle shader
// Demonstrates per-vertex colors with transformation matrices

import math;

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
    output.position = u_projection_matrix * u_view_matrix * u_world_matrix * float4(input.i_position, 1.0);

    return output;
}

[shader("fragment")]
float4 fragmentMain(VertexVaryingData input) : SV_Target
{
    return input.color_0;
}
