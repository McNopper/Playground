#include "helper.h"

#define _USE_MATH_DEFINES
#include <cmath>

float degreeToRadians(float degree)
{
	return 2.0f * (float)M_PI * degree / 360.0f;
}

float radiansToDegree(float radians)
{
	return 360.0f * radians / (2.0f * (float)M_PI);
}
