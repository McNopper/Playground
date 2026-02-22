#include "matrix_camera.h"

#include <cmath>

#include "helper.h"

float4x4 lookAt(const float3& eye, const float3& center, const float3& up)
{
    float3 forward = normalize(center - eye);
    float3 right = normalize(cross(forward, up));
    float3 corrected_up = cross(right, forward);

    return {
        {
            right[0],
            corrected_up[0],
            -forward[0],
            0.0f
        },
        {
            right[1],
            corrected_up[1],
            -forward[1],
            0.0f
        },
        {
            right[2],
            corrected_up[2],
            -forward[2],
            0.0f
        },
        {
            -dot(right, eye),
            -dot(corrected_up, eye),
            dot(forward, eye),
            1.0f
        }
    };
}

float4x4 frustum(float left, float right, float bottom, float top, float near_val, float far_val)
{
    // Vulkan NDC: x:[-1,1], y:[-1,1], z:[0,1]
    // Vulkan has Y-down in NDC: -1 = top, +1 = bottom
    // Use negative Y-scale to flip from Y-up world space to Y-down NDC
    return {
        {
            2.0f * near_val / (right - left),
            0.0f,
            0.0f,
            0.0f
        },
        {
            0.0f,
            -2.0f * near_val / (top - bottom),
            0.0f,
            0.0f
        },
        {
            (right + left) / (right - left),
            (top + bottom) / (top - bottom),
            far_val / (near_val - far_val),
            -1.0f
        },
        {
            0.0f,
            0.0f,
            (near_val * far_val) / (near_val - far_val),
            0.0f
        }
    };
}

float4x4 orthographic(float left, float right, float bottom, float top, float near_val, float far_val)
{
    // Vulkan has Y-down in NDC: -1 = top, +1 = bottom
    // Use negative Y-scale to flip from Y-up world space to Y-down NDC
    return {
        {
            2.0f / (right - left),
            0.0f,
            0.0f,
            0.0f
        },
        {
            0.0f,
            -2.0f / (top - bottom),
            0.0f,
            0.0f
        },
        {
            0.0f,
            0.0f,
            -1.0f / (far_val - near_val),
            0.0f
        },
        {
            -(right + left) / (right - left),
            -(top + bottom) / (top - bottom),
            -near_val / (far_val - near_val),
            1.0f
        }
    };
}

float4x4 perspective(float fov_y, float aspect, float near_val, float far_val)
{
    float y_max = near_val * std::tanf(degreeToRadians(fov_y * 0.5f));
    float y_min = -y_max;
    float x_min = y_min * aspect;
    float x_max = y_max * aspect;

    return frustum(x_min, x_max, y_min, y_max, near_val, far_val);
}

float4x4 viewport(float x, float y, float width, float height, float near_val, float far_val)
{
    return {
        {
            width * 0.5f,
            0.0f,
            0.0f,
            0.0f
        },
        {
            0.0f,
            height * 0.5f,
            0.0f,
            0.0f
        },
        {
            0.0f,
            0.0f,
            // z range is [0 1]
            far_val - near_val,
            0.0f
        },
        {
            x + width * 0.5f,
            y + height * 0.5f,
            // z range is [0 1]
            near_val,
            1.0f
        }
    };
}
