#ifndef CORE_MATH_MATRIXCAMERA_H_
#define CORE_MATH_MATRIXCAMERA_H_

#include "matrix.h"
#include "vector.h"

float4x4 lookAt(const float3& eye, const float3& center, const float3& up);

float4x4 frustum(float left, float right, float bottom, float top, float near_val, float far_val);

float4x4 orthographic(float left, float right, float bottom, float top, float near_val, float far_val);

float4x4 perspective(float fov_y, float aspect, float near_val, float far_val);

float4x4 viewport(float x, float y, float width, float height, float near_val, float far_val);

#endif /* CORE_MATH_MATRIXCAMERA_H_ */
