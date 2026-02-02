#ifndef ENGINE_RENDERER_SCENE_UNIFORMDATA_H_
#define ENGINE_RENDERER_SCENE_UNIFORMDATA_H_

#include "core/math/vector.h"
#include "core/math/matrix.h"

/// <summary>
/// Standard uniform buffer for view-space transformations (camera).
/// Matches shader cbuffer UniformViewData layout.
/// Follows MaterialX naming: u_ prefix with camelCase.
/// </summary>
struct UniformViewData
{
	float4x4 u_projectionMatrix{ 1.0f };
	float4x4 u_viewMatrix{ 1.0f };
};

/// <summary>
/// Standard uniform buffer for model-space transformations (object).
/// Matches shader cbuffer UniformModelData layout.
/// Follows MaterialX naming: u_ prefix with camelCase.
/// </summary>
struct UniformModelData
{
	float4x4 u_worldMatrix{ 1.0f };
};

#endif // ENGINE_RENDERER_SCENE_UNIFORMDATA_H_
