// ibl_specular.slang - Specular IBL prefiltered environment map
// Generates one mip level of the prefiltered environment cube map using
// GGX importance sampling. Run once per mip level with varying roughness.
//
// Dispatch: (ceil(width/8), ceil(height/8), 6)
// threadId.z selects the cube face (Vulkan convention: 0=+X .. 5=-Z)

import pbr;
import sampling;
import geometry;

Texture2D<float4> u_env_map;
SamplerState u_env_sampler;
RWTexture2DArray<float4> u_output_cube;

struct PushConstants
{
    float roughness;
    uint sample_count;
};
[[vk::push_constant]]
PushConstants push;

[shader("compute")]
[numthreads(8, 8, 1)]
void computeMain(uint3 thread_id : SV_DispatchThreadID)
{
    uint width, height, layers;
    u_output_cube.GetDimensions(width, height, layers);

    if (thread_id.x >= width || thread_id.y >= height || thread_id.z >= 6u)
        return;

    float2 uv = (float2(thread_id.xy) + 0.5) / float2(width, height);
    float3 r = cubeFaceUVToDirection(thread_id.z, uv);
    float3 n = r;
    float3 v = r;

    float3 prefiltered_color = float3(0.0);
    float total_weight = 0.0;

    for (uint i = 0u; i < push.sample_count; ++i)
    {
        float2 xi = hammersley(i, push.sample_count);
        float3 h  = importanceSampleGGX(xi, n, push.roughness);
        float3 l  = normalize(2.0 * dot(v, h) * h - v);

        float n_dot_l = max(dot(n, l), 0.0);
        if (n_dot_l > 0.0)
        {
            float2 env_uv = directionToEquirectangularUV(l);
            prefiltered_color += u_env_map.SampleLevel(u_env_sampler, env_uv, 0.0).rgb * n_dot_l;
            total_weight += n_dot_l;
        }
    }

    if (total_weight > 0.0)
        prefiltered_color /= total_weight;

    u_output_cube[uint3(thread_id.xy, thread_id.z)] = float4(prefiltered_color, 1.0);
}
