#ifndef CORE_MATH_DEBUG_H_
#define CORE_MATH_DEBUG_H_

#include <string>

#include "matrix.h"
#include "quaternion.h"
#include "vector.h"

std::string toString(const float2& value);

std::string toString(const float3& value);

std::string toString(const float4& value);

std::string toString(const float2x2& value);

std::string toString(const float3x3& value);

std::string toString(const float4x4& value);

std::string toString(const quaternion& value);

#endif /* CORE_MATH_DEBUG_H_ */
