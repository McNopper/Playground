#ifndef CORE_MATH_RSH_ROTATION_H_
#define CORE_MATH_RSH_ROTATION_H_

struct float3;
struct float3x3;
struct float5;
struct float5x5;
struct float7;
struct float7x7;

//
// Spherical Harmonics Rotation (Bands 0-3)
// 
// Extracted and adapted from sh-lib by Andrew Willmott
// https://github.com/andrewwillmott/sh-lib
//
// This module provides analytical rotation of spherical harmonic coefficients
// for bands 0-3 (degree 0-3), supporting up to 16 coefficients total.
//
// The rotation uses analytical formulas derived from SH properties, which is
// much faster than sampling and projecting back to SH space.
//
// References:
// - Ivanic, J. and Ruedenberg, K. (1996). "Rotation Matrices for Real Spherical
//   Harmonics. Direct Determination by Recursion", Journal of Physical Chemistry,
//   100(15), 6342-6347. https://doi.org/10.1021/jp9833350
// - Green, R. (2003). "Spherical Harmonic Lighting: The Gritty Details"
//

//
// Build SH rotation matrices
//
// These functions construct the rotation matrices needed for each band.
// Band 1 depends on the input rotation matrix.
// Band 2 depends on band 1's matrix.
// Band 3 depends on both band 1 and band 2 matrices.
//

// Build 3x3 rotation matrix for band 1 from a 3D rotation matrix
float3x3 buildSH1Matrix(const float3x3& rotation_matrix);

// Build 5x5 rotation matrix for band 2 from band 1's rotation matrix
float5x5 buildSH2Matrix(const float3x3& sh1);

// Build 7x7 rotation matrix for band 3 from band 1 and band 2's rotation matrices
float7x7 buildSH3Matrix(const float3x3& sh1, const float5x5& sh2);

//
// Apply SH rotation matrices to coefficients
//
// These functions apply pre-computed rotation matrices to SH coefficients.
// After building the rotation matrices once, these can be called multiple times
// for different coefficient sets, making batch operations efficient.
//

// Band 0: Constant term (no rotation needed, just returns input)
float rotateBand0(float coefficient_in);

// Band 1: Apply 3x3 rotation matrix to 3 coefficients
float3 rotateBand1(const float3x3& sh1, const float3& coefficients_in);

// Band 2: Apply 5x5 rotation matrix to 5 coefficients
float5 rotateBand2(const float5x5& sh2, const float5& coefficients_in);

// Band 3: Apply 7x7 rotation matrix to 7 coefficients
float7 rotateBand3(const float7x7& sh3, const float7& coefficients_in);

#endif /* CORE_MATH_RSH_ROTATION_H_ */
