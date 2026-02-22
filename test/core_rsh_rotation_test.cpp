#include <gtest/gtest.h>

#include "core/core.h"

TEST(TestRSHRotation, Band0Identity)
{
    // Band 0 should always return the input unchanged (rotation invariant)
    float coeff_in = 1.5f;
    float coeff_out = rotateBand0(coeff_in);

    EXPECT_FLOAT_EQ(coeff_out, coeff_in);
}

TEST(TestRSHRotation, Band1Identity)
{
    // Identity rotation should preserve coefficients
    float3x3 identity = float3x3(1.0f); // Identity: diagonal ones
    identity[0].x = 1.0f;
    identity[1].y = 1.0f;
    identity[2].z = 1.0f;
    float3x3 sh1 = buildSH1Matrix(identity);

    float3 coeffs_in{ 1.0f, 2.0f, 3.0f };
    float3 coeffs_out = rotateBand1(sh1, coeffs_in);

    EXPECT_NEAR(coeffs_out.x, coeffs_in.x, 0.0001f);
    EXPECT_NEAR(coeffs_out.y, coeffs_in.y, 0.0001f);
    EXPECT_NEAR(coeffs_out.z, coeffs_in.z, 0.0001f);
}

TEST(TestRSHRotation, Band1Rotation90Y)
{
    // 90 degree rotation around Y axis (extract 3x3 from 4x4)
    float3x3 rotation = float3x3(rotateRyMatrix(90.0f));
    float3x3 sh1 = buildSH1Matrix(rotation);

    // Input: Test with simple coefficients
    float3 coeffs_in{ 1.0f, 2.0f, 3.0f };
    float3 coeffs_out = rotateBand1(sh1, coeffs_in);

    // Verify magnitude is preserved by rotation
    float mag_in = length(coeffs_in);
    float mag_out = length(coeffs_out);
    EXPECT_NEAR(mag_in, mag_out, 0.0001f);
}

TEST(TestRSHRotation, Band1Rotation90X)
{
    // 90 degree rotation around X axis (extract 3x3 from 4x4)
    float3x3 rotation = float3x3(rotateRxMatrix(90.0f));
    float3x3 sh1 = buildSH1Matrix(rotation);

    // Input: Test with simple coefficients
    float3 coeffs_in{ 1.0f, 2.0f, 3.0f };
    float3 coeffs_out = rotateBand1(sh1, coeffs_in);

    // Verify magnitude is preserved by rotation
    float mag_in = length(coeffs_in);
    float mag_out = length(coeffs_out);
    EXPECT_NEAR(mag_in, mag_out, 0.0001f);
}

TEST(TestRSHRotation, Band2Identity)
{
    // Identity rotation should preserve coefficients
    float3x3 identity = float3x3(1.0f);
    identity[0].x = 1.0f;
    identity[1].y = 1.0f;
    identity[2].z = 1.0f;
    float3x3 sh1 = buildSH1Matrix(identity);
    float5x5 sh2 = buildSH2Matrix(sh1);

    float5 coeffs_in{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
    float5 coeffs_out = rotateBand2(sh2, coeffs_in);

    EXPECT_NEAR(coeffs_out.x, coeffs_in.x, 0.0001f);
    EXPECT_NEAR(coeffs_out.y, coeffs_in.y, 0.0001f);
    EXPECT_NEAR(coeffs_out.z, coeffs_in.z, 0.0001f);
    EXPECT_NEAR(coeffs_out.w, coeffs_in.w, 0.0001f);
    EXPECT_NEAR(coeffs_out.v, coeffs_in.v, 0.0001f);
}

TEST(TestRSHRotation, Band3Identity)
{
    // Identity rotation should preserve coefficients
    float3x3 identity = float3x3(1.0f);
    identity[0].x = 1.0f;
    identity[1].y = 1.0f;
    identity[2].z = 1.0f;
    float3x3 sh1 = buildSH1Matrix(identity);
    float5x5 sh2 = buildSH2Matrix(sh1);
    float7x7 sh3 = buildSH3Matrix(sh1, sh2);

    float7 coeffs_in{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f };
    float7 coeffs_out = rotateBand3(sh3, coeffs_in);

    EXPECT_NEAR(coeffs_out.x, coeffs_in.x, 0.0001f);
    EXPECT_NEAR(coeffs_out.y, coeffs_in.y, 0.0001f);
    EXPECT_NEAR(coeffs_out.z, coeffs_in.z, 0.0001f);
    EXPECT_NEAR(coeffs_out.w, coeffs_in.w, 0.0001f);
    EXPECT_NEAR(coeffs_out.v, coeffs_in.v, 0.0001f);
    EXPECT_NEAR(coeffs_out.u, coeffs_in.u, 0.0001f);
    EXPECT_NEAR(coeffs_out.t, coeffs_in.t, 0.0001f);
}

TEST(TestRSHRotation, Band1DoubleRotation)
{
    // Rotating twice by 90° Y should equal rotating by 180° Y
    float3x3 rotation90 = float3x3(rotateRyMatrix(90.0f));
    float3x3 rotation180 = float3x3(rotateRyMatrix(180.0f));
    float3x3 sh1_90 = buildSH1Matrix(rotation90);
    float3x3 sh1_180 = buildSH1Matrix(rotation180);

    float3 coeffs_in{ 1.0f, 2.0f, 3.0f };
    float3 coeffs_double = rotateBand1(sh1_90, rotateBand1(sh1_90, coeffs_in));
    float3 coeffs_direct = rotateBand1(sh1_180, coeffs_in);

    // Both approaches should give same result (within tolerance)
    EXPECT_NEAR(coeffs_double.x, coeffs_direct.x, 0.001f);
    EXPECT_NEAR(coeffs_double.y, coeffs_direct.y, 0.001f);
    EXPECT_NEAR(coeffs_double.z, coeffs_direct.z, 0.001f);
}

TEST(TestRSHRotation, Band2MatrixOrthogonality)
{
    // Rotation matrices should be orthogonal: M * M^T = I
    float3x3 rotation = float3x3(rotateRzMatrix(45.0f));
    float3x3 sh1 = buildSH1Matrix(rotation);
    float5x5 sh2 = buildSH2Matrix(sh1);
    float5x5 sh2_T = transpose(sh2);
    float5x5 product = sh2 * sh2_T;

    // Check diagonal elements are close to 1
    EXPECT_NEAR(product[0][0], 1.0f, 0.0001f);
    EXPECT_NEAR(product[1][1], 1.0f, 0.0001f);
    EXPECT_NEAR(product[2][2], 1.0f, 0.0001f);
    EXPECT_NEAR(product[3][3], 1.0f, 0.0001f);
    EXPECT_NEAR(product[4][4], 1.0f, 0.0001f);
}

TEST(TestRSHRotation, Band3MatrixOrthogonality)
{
    // Rotation matrices should be orthogonal: M * M^T = I
    float3x3 rotation = float3x3(rotateRxMatrix(30.0f));
    float3x3 sh1 = buildSH1Matrix(rotation);
    float5x5 sh2 = buildSH2Matrix(sh1);
    float7x7 sh3 = buildSH3Matrix(sh1, sh2);
    float7x7 sh3_T = transpose(sh3);
    float7x7 product = sh3 * sh3_T;

    // Check diagonal elements are close to 1
    EXPECT_NEAR(product[0][0], 1.0f, 0.001f);
    EXPECT_NEAR(product[1][1], 1.0f, 0.001f);
    EXPECT_NEAR(product[2][2], 1.0f, 0.001f);
    EXPECT_NEAR(product[3][3], 1.0f, 0.001f);
    EXPECT_NEAR(product[4][4], 1.0f, 0.001f);
    EXPECT_NEAR(product[5][5], 1.0f, 0.001f);
    EXPECT_NEAR(product[6][6], 1.0f, 0.001f);
}

TEST(TestRSHRotation, Band1MagnitudePreservation)
{
    // Rotation should preserve the magnitude of coefficient vectors
    float3x3 rotation = float3x3(rotateRzMatrix(37.0f));
    float3x3 sh1 = buildSH1Matrix(rotation);

    float3 coeffs_in{ 1.0f, 2.0f, 3.0f };
    float3 coeffs_out = rotateBand1(sh1, coeffs_in);

    float mag_in = length(coeffs_in);
    float mag_out = length(coeffs_out);

    EXPECT_NEAR(mag_in, mag_out, 0.0001f);
}

TEST(TestRSHRotation, Band2MagnitudePreservation)
{
    // Rotation should preserve the magnitude of coefficient vectors
    float3x3 rotation = float3x3(rotateRyMatrix(67.0f));
    float3x3 sh1 = buildSH1Matrix(rotation);
    float5x5 sh2 = buildSH2Matrix(sh1);

    float5 coeffs_in{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
    float5 coeffs_out = rotateBand2(sh2, coeffs_in);

    float mag_in = length(coeffs_in);
    float mag_out = length(coeffs_out);

    EXPECT_NEAR(mag_in, mag_out, 0.001f);
}

TEST(TestRSHRotation, Band3MagnitudePreservation)
{
    // Rotation should preserve the magnitude of coefficient vectors
    float3x3 rotation = float3x3(rotateRxMatrix(123.0f));
    float3x3 sh1 = buildSH1Matrix(rotation);
    float5x5 sh2 = buildSH2Matrix(sh1);
    float7x7 sh3 = buildSH3Matrix(sh1, sh2);

    float7 coeffs_in{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f };
    float7 coeffs_out = rotateBand3(sh3, coeffs_in);

    float mag_in = length(coeffs_in);
    float mag_out = length(coeffs_out);

    EXPECT_NEAR(mag_in, mag_out, 0.001f);
}

TEST(TestRSHRotation, BuildSH1MatrixYZXOrdering)
{
    // Verify that buildSH1Matrix extracts in (Y, Z, X) column order with (y, z, x) component order
    float3x3 rotation = float3x3{
        { 1.0f, 2.0f, 3.0f }, // Column 0 (X in standard rotation)
        { 4.0f, 5.0f, 6.0f }, // Column 1 (Y in standard rotation)
        { 7.0f, 8.0f, 9.0f }  // Column 2 (Z in standard rotation)
    };

    float3x3 sh1 = buildSH1Matrix(rotation);

    // sh1[col][row] extracts rotation_matrix columns Y,Z,X with components y,z,x
    // Column 0: Y column (rotation[1]) with (y, z, x) components
    EXPECT_FLOAT_EQ(sh1[0].x, rotation[1].y); // 5.0f
    EXPECT_FLOAT_EQ(sh1[0].y, rotation[2].y); // 8.0f
    EXPECT_FLOAT_EQ(sh1[0].z, rotation[0].y); // 2.0f

    // Column 1: Z column (rotation[2]) with (y, z, x) components
    EXPECT_FLOAT_EQ(sh1[1].x, rotation[1].z); // 6.0f
    EXPECT_FLOAT_EQ(sh1[1].y, rotation[2].z); // 9.0f
    EXPECT_FLOAT_EQ(sh1[1].z, rotation[0].z); // 3.0f

    // Column 2: X column (rotation[0]) with (y, z, x) components
    EXPECT_FLOAT_EQ(sh1[2].x, rotation[1].x); // 4.0f
    EXPECT_FLOAT_EQ(sh1[2].y, rotation[2].x); // 7.0f
    EXPECT_FLOAT_EQ(sh1[2].z, rotation[0].x); // 1.0f
}
