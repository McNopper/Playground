// ibl_diffuse.slang - Diffuse IBL irradiance convolution
// Convolves an equirectangular environment map into a cube map irradiance map
// using a cosine-weighted Riemann sum over the hemisphere.
//
// Dispatch: (ceil(width/8), ceil(height/8), 6)
// threadId.z selects the cube face (Vulkan convention: 0=+X .. 5=-Z)

import math;
import geometry;

Texture2D<float4> u_env_map;
SamplerState u_env_sampler;
RWTexture2DArray<float4> u_output_cube;

[shader("compute")]
[numthreads(8, 8, 1)]
void computeMain(uint3 thread_id : SV_DispatchThreadID)
{
    uint width, height, layers;
    u_output_cube.GetDimensions(width, height, layers);

    if (thread_id.x >= width || thread_id.y >= height || thread_id.z >= 6u)
        return;

    float2 uv = (float2(thread_id.xy) + 0.5) / float2(width, height);
    float3 normal = cubeFaceUVToDirection(thread_id.z, uv);

    // Build orthonormal tangent frame around the face normal
    float3 up = abs(normal.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    float3 right = normalize(cross(up, normal));
    float3 fwd   = cross(normal, right);

    // Cosine-weighted hemisphere Riemann sum
    float3 irradiance = float3(0.0);
    float sample_count = 0.0;
    const float delta = 0.025;

    for (float phi = 0.0; phi < TWO_PI; phi += delta)
    {
        for (float theta = 0.0; theta < 0.5 * PI; theta += delta)
        {
            float sin_theta = sin(theta);
            float cos_theta = cos(theta);

            float3 tangent_sample = float3(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta);
            float3 sample_dir = tangent_sample.x * right + tangent_sample.y * fwd + tangent_sample.z * normal;

            float2 env_uv = directionToEquirectangularUV(sample_dir);
            irradiance += u_env_map.SampleLevel(u_env_sampler, env_uv, 0.0).rgb * cos_theta * sin_theta;
            sample_count += 1.0;
        }
    }

    irradiance = PI * irradiance / sample_count;
    u_output_cube[uint3(thread_id.xy, thread_id.z)] = float4(irradiance, 1.0);
}
