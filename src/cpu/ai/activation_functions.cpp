#include "activation_functions.h"

#include <cmath>

float identity(float x)
{
    return x;
}

float identityDerivative(float x)
{
    return 1.0f;
}

float binaryStep(float x)
{
    if (x < 0.0f)
    {
        return 0.0f;
    }

    return 1.0f;
}

float binaryStepDerivative(float x)
{
    return 0.0f;
}

float sigmoid(float x)
{
    return 1.0f / (1.0f + std::exp(-x));
}

float sigmoidDerivative(float x)
{
    return x * (1.0f - x);
}

float hyperbolicTangent(float x)
{
    return std::tanh(x);
}

float hyperbolicTangentDerivative(float x)
{
    return 1.0f - (x * x);
}

float rectifiedLinearUnit(float x)
{
    if (x < 0.0f)
    {
        return 0.0f;
    }

    return x;
}

float rectifiedLinearUnitDerivative(float x)
{
    if (x <= 0.0f)
    {
        return 0.0f;
    }

    return 1.0f;
}

float leakyRectifiedLinearUnit(float x)
{
    if (x < 0.0f)
    {
        return 0.01f * x;
    }

    return x;
}

float leakyRectifiedLinearUnitDerivative(float x)
{
    if (x <= 0.0f)
    {
        return 0.01f;
    }

    return 1.0f;
}

float exponentialLinearUnit(float x)
{
    if (x < 0.0f)
    {
        return std::expm1(x);
    }

    return x;
}

float exponentialLinearUnitDerivative(float x)
{
    if (x <= 0.0f)
    {
        return x + 1.0f;
    }

    return 1.0f;
}

float swish(float x)
{
    return x / (1.0f + std::exp(-x));
}

float swishDerivative(float x)
{
    float sigmoid_x = 1.0f / (1.0f + std::exp(-x));
    return x * sigmoid_x * (1.0f - sigmoid_x) + sigmoid_x;
}

float gaussianErrorLinearUnit(float x)
{
    const float sqrt_2_over_pi{ 0.7978845608f };
    const float coeff{ 0.044715f };
    float x_cubed{ x * x * x };
    float tanh_arg{ sqrt_2_over_pi * (x + coeff * x_cubed) };
    return 0.5f * x * (1.0f + std::tanh(tanh_arg));
}

float gaussianErrorLinearUnitDerivative(float x)
{
    const float sqrt_2_over_pi{ 0.7978845608f };
    const float coeff{ 0.044715f };
    float x_squared{ x * x };
    float x_cubed{ x_squared * x };
    float tanh_arg{ sqrt_2_over_pi * (x + coeff * x_cubed) };
    float tanh_val{ std::tanh(tanh_arg) };
    float sech_squared{ 1.0f - tanh_val * tanh_val };
    float tanh_derivative{ sqrt_2_over_pi * (1.0f + 3.0f * coeff * x_squared) * sech_squared };
    return 0.5f * (1.0f + tanh_val) + 0.5f * x * tanh_derivative;
}
