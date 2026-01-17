#include "quaternion_transform.h"

#include <cmath>

#include "helper.h"

quaternion rotateRxQuaternion(float degree)
{
	float hr = degreeToRadians(degree) * 0.5f;

	return {
		std::sin(hr),
		0.0f,
		0.0f,
		std::cos(hr)
	};
}

quaternion rotateRyQuaternion(float degree)
{
	float hr = degreeToRadians(degree) * 0.5f;

	return {
		0.0f,
		std::sin(hr),
		0.0f,
		std::cos(hr)
	};
}

quaternion rotateRzQuaternion(float degree)
{
	float hr = degreeToRadians(degree) * 0.5f;

	return {
		0.0f,
		0.0f,
		std::sin(hr),
		std::cos(hr)
	};
}
