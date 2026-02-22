#ifndef CORE_MATH_MATRIXTRANSFORM_H_
#define CORE_MATH_MATRIXTRANSFORM_H_

#include "matrix.h"
#include "vector.h"

float4x4 translationMatrix(const float3& t);

float4x4 scaleMatrix(const float3& s);

float4x4 rotateRxMatrix(float degree);

float4x4 rotateRyMatrix(float degree);

float4x4 rotateRzMatrix(float degree);

#endif /* CORE_MATH_MATRIXTRANSFORM_H_ */
