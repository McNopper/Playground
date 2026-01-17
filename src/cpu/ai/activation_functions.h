#ifndef CPU_AI_ACTIVATIONFUNCTIONS_H_
#define CPU_AI_ACTIVATIONFUNCTIONS_H_

#include <vector>

// Only supporting one fold x
// see https://en.wikipedia.org/wiki/Activation_function

float identity(float x);
float identityDerivative(float x);

float binaryStep(float x);
float binaryStepDerivative(float x);

// also known as Logistic
float sigmoid(float x);
float sigmoidDerivative(float x);

float hyperbolicTangent(float x);
float hyperbolicTangentDerivative(float x);

// in short ReLU
float rectifiedLinearUnit(float x);
float rectifiedLinearUnitDerivative(float x);

// in short Leaky ReLU
float leakyRectifiedLinearUnit(float x);
float leakyRectifiedLinearUnitDerivative(float x);

// in short ELU
float exponentialLinearUnit(float x);
float exponentialLinearUnitDerivative(float x);

// also known as SiLU
float swish(float x);
float swishDerivative(float x);

// Gaussian Error Linear Unit
float gaussianErrorLinearUnit(float x);
float gaussianErrorLinearUnitDerivative(float x);

#endif /* CPU_AI_ACTIVATIONFUNCTIONS_H_ */
