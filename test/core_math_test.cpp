#include <gtest/gtest.h>

#include "core/core.h"

TEST(TestMath, Float3Cross)
{
    float3 a{ 1.0f, 0.0f, 0.0f };
    float3 b{ 0.0f, 1.0f, 0.0f };

    auto c = cross(a, b);

    EXPECT_FLOAT_EQ(c.x, 0.0f);
    EXPECT_FLOAT_EQ(c.y, 0.0f);
    EXPECT_FLOAT_EQ(c.z, 1.0f);
}

TEST(TestMath, Float2x2Determinant)
{
    float2x2 a{ {1.0f, 2.0f}, {3.0f, 4.0f} };

    auto b = determinant(a);

    EXPECT_FLOAT_EQ(b, -2.0f);
}

TEST(TestMath, Float2x2Transpose)
{
    float2x2 a{ {1.0f, 2.0f}, {3.0f, 4.0f} };

    auto b = transpose(a);

    EXPECT_FLOAT_EQ(b[0].x, 1.0f);
    EXPECT_FLOAT_EQ(b[0].y, 3.0f);
    EXPECT_FLOAT_EQ(b[1].x, 2.0f);
    EXPECT_FLOAT_EQ(b[1].y, 4.0f);
}

TEST(TestMath, Float2x2Inverse)
{
	float2x2 a{ {1.0f, 2.0f}, {3.0f, 4.0f} };

	auto b = inverse(a);

    EXPECT_FLOAT_EQ(b[0].x, -2.0f);
    EXPECT_FLOAT_EQ(b[0].y, 1.0f);
    EXPECT_FLOAT_EQ(b[1].x, 1.5f);
    EXPECT_FLOAT_EQ(b[1].y, -0.5f);
}

TEST(TestMath, Translation)
{
    auto a = translationMatrix({ 1.0f, 2.0f, 3.0f }) * float4{ 0.0f, 0.0f, 0.0f, 1.0f };

    EXPECT_FLOAT_EQ(a.x, 1.0f);
    EXPECT_FLOAT_EQ(a.y, 2.0f);
    EXPECT_FLOAT_EQ(a.z, 3.0f);
    EXPECT_FLOAT_EQ(a.w, 1.0f);
}

TEST(TestMath, Scale)
{
    auto a = scaleMatrix({ 1.0f, 2.0f, 3.0f }) * float4 { 1.0f, 1.0f, 1.0f, 1.0f };

    EXPECT_FLOAT_EQ(a.x, 1.0f);
    EXPECT_FLOAT_EQ(a.y, 2.0f);
    EXPECT_FLOAT_EQ(a.z, 3.0f);
    EXPECT_FLOAT_EQ(a.w, 1.0f);
}

TEST(TestMath, RotationX)
{
    auto a = rotateRxMatrix(90.0f) * float4 { 0.0f, 1.0f, 0.0f, 1.0f };

    EXPECT_NEAR(a.x, 0.0f, 0.000001f);
    EXPECT_NEAR(a.y, 0.0f, 0.000001f);
    EXPECT_NEAR(a.z, 1.0f, 0.000001f);
    EXPECT_NEAR(a.w, 1.0f, 0.000001f);
}

TEST(TestMath, RotationY)
{
    auto a = rotateRyMatrix(90.0f) * float4 { 1.0f, 0.0f, 0.0f, 1.0f };

    EXPECT_NEAR(a.x, 0.0f, 0.000001f);
    EXPECT_NEAR(a.y, 0.0f, 0.000001f);
    EXPECT_NEAR(a.z, -1.0f, 0.000001f);
    EXPECT_NEAR(a.w, 1.0f, 0.000001f);
}

TEST(TestMath, RotationZ)
{
    auto a = rotateRzMatrix(90.0f) * float4 { 1.0f, 0.0f, 0.0f, 1.0f };

    EXPECT_NEAR(a.x, 0.0f, 0.000001f);
    EXPECT_NEAR(a.y, 1.0f, 0.000001f);
    EXPECT_NEAR(a.z, 0.0f, 0.000001f);
    EXPECT_NEAR(a.w, 1.0f, 0.000001f);
}

TEST(TestMath, Multiplication01)
{
    auto a = float2{ 1.0, 0.0 } * float2x2{ { 1.0f, 2.0f }, { 3.0f, 4.0f } };

    EXPECT_FLOAT_EQ(a.x, 1.0f);
    EXPECT_FLOAT_EQ(a.y, 3.0f);
}

TEST(TestMath, Multiplication02)
{
    auto a = float2x2{ { 1.0f, 2.0f }, { 3.0f, 4.0f } } * float2{ 1.0, 0.0 };

    EXPECT_FLOAT_EQ(a.x, 1.0f);
    EXPECT_FLOAT_EQ(a.y, 2.0f);
}

TEST(TestMath, Interpolate01)
{
    float edge0{ 0.0f };
    float edge1{ 2.0f };
    float x{ 1.0f };

    float y = smoothstep(edge0, edge1, x);

    EXPECT_NEAR(y, 0.5f, 0.000001f);
}

TEST(TestMath, LookAtBasic)
{
    // Camera at (0, 0, 5) looking at origin
    auto view = lookAt(float3{ 0.0f, 0.0f, 5.0f }, float3{ 0.0f, 0.0f, 0.0f }, float3{ 0.0f, 1.0f, 0.0f });
    
    // Transform origin point to camera space - should be at (0, 0, -5) in camera space
    auto origin_in_camera = view * float4{ 0.0f, 0.0f, 0.0f, 1.0f };
    
    EXPECT_NEAR(origin_in_camera.x, 0.0f, 0.000001f);
    EXPECT_NEAR(origin_in_camera.y, 0.0f, 0.000001f);
    EXPECT_NEAR(origin_in_camera.z, -5.0f, 0.000001f);
    EXPECT_NEAR(origin_in_camera.w, 1.0f, 0.000001f);
}

TEST(TestMath, LookAtIdentity)
{
    // Camera at origin looking down -Z axis (OpenGL convention)
    auto view = lookAt(float3{ 0.0f, 0.0f, 0.0f }, float3{ 0.0f, 0.0f, -1.0f }, float3{ 0.0f, 1.0f, 0.0f });
    
    // A point in front of camera should have negative Z in camera space
    auto point = view * float4{ 0.0f, 0.0f, -5.0f, 1.0f };
    
    EXPECT_NEAR(point.x, 0.0f, 0.000001f);
    EXPECT_NEAR(point.y, 0.0f, 0.000001f);
    EXPECT_NEAR(point.z, -5.0f, 0.000001f);
    EXPECT_NEAR(point.w, 1.0f, 0.000001f);
}

TEST(TestMath, PerspectiveBasic)
{
    // 90 degree FOV, aspect 1:1, near 1, far 10
    auto proj = perspective(90.0f, 1.0f, 1.0f, 10.0f);
    
    // Point at center of near plane should map to (0, 0, 0) in NDC
    auto near_center = proj * float4{ 0.0f, 0.0f, -1.0f, 1.0f };
    near_center.x /= near_center.w;
    near_center.y /= near_center.w;
    near_center.z /= near_center.w;
    
    EXPECT_NEAR(near_center.x, 0.0f, 0.000001f);
    EXPECT_NEAR(near_center.y, 0.0f, 0.000001f);
    EXPECT_NEAR(near_center.z, 0.0f, 0.000001f); // Vulkan depth range [0, 1]
}

TEST(TestMath, PerspectiveFarPlane)
{
    auto proj = perspective(90.0f, 1.0f, 1.0f, 10.0f);
    
    // Point at center of far plane should map to z=1 in NDC (Vulkan)
    auto far_center = proj * float4{ 0.0f, 0.0f, -10.0f, 1.0f };
    far_center.x /= far_center.w;
    far_center.y /= far_center.w;
    far_center.z /= far_center.w;
    
    EXPECT_NEAR(far_center.x, 0.0f, 0.000001f);
    EXPECT_NEAR(far_center.y, 0.0f, 0.000001f);
    EXPECT_NEAR(far_center.z, 1.0f, 0.000001f); // Vulkan depth range [0, 1]
}

TEST(TestMath, OrthographicBasic)
{
    // Left=-2, Right=2, Bottom=-2, Top=2, Near=1, Far=10
    auto proj = orthographic(-2.0f, 2.0f, -2.0f, 2.0f, 1.0f, 10.0f);
    
    // Center point at near plane
    auto near_center = proj * float4{ 0.0f, 0.0f, -1.0f, 1.0f };
    
    EXPECT_NEAR(near_center.x, 0.0f, 0.000001f);
    EXPECT_NEAR(near_center.y, 0.0f, 0.000001f);
    EXPECT_NEAR(near_center.z, 0.0f, 0.000001f); // Vulkan depth range [0, 1]
    EXPECT_NEAR(near_center.w, 1.0f, 0.000001f);
}

TEST(TestMath, OrthographicFarPlane)
{
    auto proj = orthographic(-2.0f, 2.0f, -2.0f, 2.0f, 1.0f, 10.0f);
    
    // Center point at far plane should map to z=1
    auto far_center = proj * float4{ 0.0f, 0.0f, -10.0f, 1.0f };
    
    EXPECT_NEAR(far_center.x, 0.0f, 0.000001f);
    EXPECT_NEAR(far_center.y, 0.0f, 0.000001f);
    EXPECT_NEAR(far_center.z, 1.0f, 0.000001f); // Vulkan depth range [0, 1]
    EXPECT_NEAR(far_center.w, 1.0f, 0.000001f);
}

TEST(TestMath, QuaternionToMatrix)
{
    // Identity quaternion
    quaternion q{ 0.0f, 0.0f, 0.0f, 1.0f };
    float4x4 m = q;
    
    // Should produce identity matrix
    auto p = m * float4{ 1.0f, 2.0f, 3.0f, 1.0f };
    
    EXPECT_NEAR(p.x, 1.0f, 0.000001f);
    EXPECT_NEAR(p.y, 2.0f, 0.000001f);
    EXPECT_NEAR(p.z, 3.0f, 0.000001f);
    EXPECT_NEAR(p.w, 1.0f, 0.000001f);
}

TEST(TestMath, QuaternionRotation90Y)
{
    // 90 degree rotation around Y axis
    quaternion q = rotateRyQuaternion(90.0f);
    float4x4 m = q;
    
    // X axis should rotate to -Z axis
    auto p = m * float4{ 1.0f, 0.0f, 0.0f, 1.0f };
    
    EXPECT_NEAR(p.x, 0.0f, 0.000001f);
    EXPECT_NEAR(p.y, 0.0f, 0.000001f);
    EXPECT_NEAR(p.z, -1.0f, 0.000001f);
    EXPECT_NEAR(p.w, 1.0f, 0.000001f);
}

