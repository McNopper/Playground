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
    float2x2 a{ { 1.0f, 2.0f }, { 3.0f, 4.0f } };

    auto b = determinant(a);

    EXPECT_FLOAT_EQ(b, -2.0f);
}

TEST(TestMath, Float2x2Transpose)
{
    float2x2 a{ { 1.0f, 2.0f }, { 3.0f, 4.0f } };

    auto b = transpose(a);

    EXPECT_FLOAT_EQ(b[0].x, 1.0f);
    EXPECT_FLOAT_EQ(b[0].y, 3.0f);
    EXPECT_FLOAT_EQ(b[1].x, 2.0f);
    EXPECT_FLOAT_EQ(b[1].y, 4.0f);
}

TEST(TestMath, Float2x2Inverse)
{
    float2x2 a{ { 1.0f, 2.0f }, { 3.0f, 4.0f } };

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
    auto a = scaleMatrix({ 1.0f, 2.0f, 3.0f }) * float4{ 1.0f, 1.0f, 1.0f, 1.0f };

    EXPECT_FLOAT_EQ(a.x, 1.0f);
    EXPECT_FLOAT_EQ(a.y, 2.0f);
    EXPECT_FLOAT_EQ(a.z, 3.0f);
    EXPECT_FLOAT_EQ(a.w, 1.0f);
}

TEST(TestMath, RotationX)
{
    auto a = rotateRxMatrix(90.0f) * float4{ 0.0f, 1.0f, 0.0f, 1.0f };

    EXPECT_NEAR(a.x, 0.0f, 0.000001f);
    EXPECT_NEAR(a.y, 0.0f, 0.000001f);
    EXPECT_NEAR(a.z, 1.0f, 0.000001f);
    EXPECT_NEAR(a.w, 1.0f, 0.000001f);
}

TEST(TestMath, RotationY)
{
    auto a = rotateRyMatrix(90.0f) * float4{ 1.0f, 0.0f, 0.0f, 1.0f };

    EXPECT_NEAR(a.x, 0.0f, 0.000001f);
    EXPECT_NEAR(a.y, 0.0f, 0.000001f);
    EXPECT_NEAR(a.z, -1.0f, 0.000001f);
    EXPECT_NEAR(a.w, 1.0f, 0.000001f);
}

TEST(TestMath, RotationZ)
{
    auto a = rotateRzMatrix(90.0f) * float4{ 1.0f, 0.0f, 0.0f, 1.0f };

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

TEST(TestMath, SphericalHarmonicsL0)
{
    rsh sh;

    // Y_0^0 is constant: sqrt(1/(4π)) ≈ 0.282095
    // Test at various directions - should always return the same value
    float z_axis = sh.Ylm(0, 0, 0.0f, 0.0f, 1.0f);
    float x_axis = sh.Ylm(0, 0, 1.0f, 0.0f, 0.0f);
    float y_axis = sh.Ylm(0, 0, 0.0f, 1.0f, 0.0f);

    EXPECT_NEAR(z_axis, 0.282095f, 0.000001f);
    EXPECT_NEAR(x_axis, 0.282095f, 0.000001f);
    EXPECT_NEAR(y_axis, 0.282095f, 0.000001f);

    // Test using n index as well
    float z_axis_n = sh.Yln(0, 0, 0.0f, 0.0f, 1.0f);
    EXPECT_NEAR(z_axis_n, 0.282095f, 0.000001f);
}

TEST(TestMath, SphericalHarmonicsL1)
{
    rsh sh;

    // Y_1^-1 = -sqrt(3/(4π)) * y ≈ -0.488603 * y
    // Y_1^0  = sqrt(3/(4π)) * z ≈ 0.488603 * z
    // Y_1^1  = -sqrt(3/(4π)) * x ≈ -0.488603 * x

    // Test along Z axis: Y_1^0 should be non-zero
    float z_pos = sh.Ylm(1, 0, 0.0f, 0.0f, 1.0f);
    EXPECT_NEAR(z_pos, 0.488603f, 0.000001f);

    // Test along X axis: Y_1^1 should be non-zero
    float x_pos = sh.Ylm(1, 1, 1.0f, 0.0f, 0.0f);
    EXPECT_NEAR(x_pos, -0.488603f, 0.000001f);

    // Test along Y axis: Y_1^-1 should be non-zero
    float y_pos = sh.Ylm(1, -1, 0.0f, 1.0f, 0.0f);
    EXPECT_NEAR(y_pos, -0.488603f, 0.000001f);

    // Test using n index (n = m + l, so for l=1, m=-1: n=0, m=0: n=1, m=1: n=2)
    float y_pos_n = sh.Yln(1, 0, 0.0f, 1.0f, 0.0f);
    EXPECT_NEAR(y_pos_n, -0.488603f, 0.000001f);
}

TEST(TestMath, SphericalHarmonicsL2)
{
    rsh sh;

    // Y_2^0 = (1/4)*sqrt(5/π) * (3z^2 - 1) ≈ 0.315392 * (3z^2 - 1)
    // Along Z axis (z=1): Y_2^0 = 0.315392 * (3 - 1) = 0.630784
    float z_axis = sh.Ylm(2, 0, 0.0f, 0.0f, 1.0f);
    EXPECT_NEAR(z_axis, 0.630784f, 0.000001f);

    // Y_2^2 = (1/4)*sqrt(15/π) * (x^2 - y^2)
    // Along X axis (x=1, y=0): Y_2^2 = 0.546274 * 1 = 0.546274
    float x_axis = sh.Ylm(2, 2, 1.0f, 0.0f, 0.0f);
    EXPECT_NEAR(x_axis, 0.546274f, 0.000001f);

    // Test using n index (for l=2, m=0: n=2, m=2: n=4)
    float z_axis_n = sh.Yln(2, 2, 0.0f, 0.0f, 1.0f);
    EXPECT_NEAR(z_axis_n, 0.630784f, 0.000001f);
}

// ============================================================================
// Plane
// ============================================================================

TEST(TestMath, PlaneCreateFromPointNormal)
{
    // XY plane: point at origin, normal pointing +Z
    Plane p = createPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f });

    EXPECT_FLOAT_EQ(p.x, 0.0f);
    EXPECT_FLOAT_EQ(p.y, 0.0f);
    EXPECT_FLOAT_EQ(p.z, 1.0f);
    EXPECT_FLOAT_EQ(p.w, 0.0f); // d = 0 since point is at origin
}

TEST(TestMath, PlaneSignedDistance)
{
    // XY plane at origin with +Z normal
    Plane p = createPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f });

    // Point in front of plane (positive side)
    EXPECT_NEAR(signedDistance(p, { 0.0f, 0.0f, 3.0f }), 3.0f, 0.000001f);

    // Point behind plane (negative side)
    EXPECT_NEAR(signedDistance(p, { 0.0f, 0.0f, -2.0f }), -2.0f, 0.000001f);

    // Point on plane
    EXPECT_NEAR(signedDistance(p, { 1.0f, 1.0f, 0.0f }), 0.0f, 0.000001f);
}

TEST(TestMath, PlaneNormalize)
{
    // Un-normalized plane: normal (0, 0, 2), d = 4 → normalized: (0, 0, 1), d = 2
    Plane p = normalize(Plane{ 0.0f, 0.0f, 2.0f, 4.0f });

    EXPECT_NEAR(p.x, 0.0f, 0.000001f);
    EXPECT_NEAR(p.y, 0.0f, 0.000001f);
    EXPECT_NEAR(p.z, 1.0f, 0.000001f);
    EXPECT_NEAR(p.w, 2.0f, 0.000001f);
}

// ============================================================================
// Sphere
// ============================================================================

TEST(TestMath, SphereContains)
{
    Sphere s{ { 0.0f, 0.0f, 0.0f }, 1.0f };

    EXPECT_TRUE(contains(s, { 0.0f, 0.0f, 0.0f }));   // center
    EXPECT_TRUE(contains(s, { 0.0f, 0.0f, 1.0f }));   // on surface
    EXPECT_FALSE(contains(s, { 0.0f, 0.0f, 2.0f }));  // outside
}

TEST(TestMath, SphereSignedDistance)
{
    Sphere s{ { 0.0f, 0.0f, 0.0f }, 1.0f };

    // Point 2 units out along Z: surface is at 1, so distance = 1
    EXPECT_NEAR(signedDistance(s, { 0.0f, 0.0f, 2.0f }), 1.0f, 0.000001f);

    // Center: distance = -radius
    EXPECT_NEAR(signedDistance(s, { 0.0f, 0.0f, 0.0f }), -1.0f, 0.000001f);
}

// ============================================================================
// AABB
// ============================================================================

TEST(TestMath, AABBContains)
{
    AABB box{ { -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 1.0f } };

    EXPECT_TRUE(contains(box, { 0.0f, 0.0f, 0.0f }));   // center
    EXPECT_TRUE(contains(box, { 1.0f, 1.0f, 1.0f }));   // corner
    EXPECT_FALSE(contains(box, { 2.0f, 0.0f, 0.0f }));  // outside on X
    EXPECT_FALSE(contains(box, { 0.0f, 0.0f, -2.0f })); // outside on Z
}

TEST(TestMath, AABBCreateFromCenterHalfExtents)
{
    AABB box = createAABB({ 1.0f, 2.0f, 3.0f }, { 0.5f, 1.0f, 1.5f });

    EXPECT_NEAR(box.min_point.x, 0.5f, 0.000001f);
    EXPECT_NEAR(box.min_point.y, 1.0f, 0.000001f);
    EXPECT_NEAR(box.min_point.z, 1.5f, 0.000001f);
    EXPECT_NEAR(box.max_point.x, 1.5f, 0.000001f);
    EXPECT_NEAR(box.max_point.y, 3.0f, 0.000001f);
    EXPECT_NEAR(box.max_point.z, 4.5f, 0.000001f);
}

TEST(TestMath, AABBCenter)
{
    AABB box{ { -2.0f, -4.0f, 0.0f }, { 4.0f, 2.0f, 6.0f } };
    float3 c = center(box);

    EXPECT_NEAR(c.x, 1.0f, 0.000001f);
    EXPECT_NEAR(c.y, -1.0f, 0.000001f);
    EXPECT_NEAR(c.z, 3.0f, 0.000001f);
}

TEST(TestMath, AABBMerge)
{
    AABB a{ { -1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f, 0.0f } };
    AABB b{ { 0.0f, 0.0f, 0.0f }, { 2.0f, 3.0f, 4.0f } };
    AABB merged = merge(a, b);

    EXPECT_NEAR(merged.min_point.x, -1.0f, 0.000001f);
    EXPECT_NEAR(merged.min_point.y, -1.0f, 0.000001f);
    EXPECT_NEAR(merged.min_point.z, -1.0f, 0.000001f);
    EXPECT_NEAR(merged.max_point.x, 2.0f, 0.000001f);
    EXPECT_NEAR(merged.max_point.y, 3.0f, 0.000001f);
    EXPECT_NEAR(merged.max_point.z, 4.0f, 0.000001f);
}

// ============================================================================
// Frustum
// ============================================================================

namespace
{

// Camera at origin looking down -Z, 90° FOV, aspect 1:1, near=1, far=100.
Frustum makeFrustum()
{
    float4x4 view = lookAt({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f });
    float4x4 proj = perspective(90.0f, 1.0f, 1.0f, 100.0f);
    return { view, proj };
}

} // namespace

TEST(TestMath, FrustumPointVisible)
{
    Frustum f = makeFrustum();

    EXPECT_TRUE(isVisible(f, { 0.0f, 0.0f, -5.0f }));  // center of frustum
    EXPECT_TRUE(isVisible(f, { 0.0f, 0.0f, -1.0f }));  // on near plane
    EXPECT_TRUE(isVisible(f, { 0.0f, 0.0f, -100.0f })); // on far plane
}

TEST(TestMath, FrustumPointNotVisible)
{
    Frustum f = makeFrustum();

    EXPECT_FALSE(isVisible(f, { 0.0f, 0.0f, 5.0f }));   // behind camera
    EXPECT_FALSE(isVisible(f, { 0.0f, 0.0f, -200.0f })); // beyond far plane
    EXPECT_FALSE(isVisible(f, { 0.0f, 0.0f, -0.5f }));   // before near plane
}

TEST(TestMath, FrustumSphereVisible)
{
    Frustum f = makeFrustum();

    EXPECT_TRUE(isVisible(f, Sphere{ { 0.0f, 0.0f, -5.0f }, 1.0f }));   // inside
    EXPECT_TRUE(isVisible(f, Sphere{ { 0.0f, 0.0f, -200.0f }, 150.0f })); // large sphere crossing far plane
}

TEST(TestMath, FrustumSphereNotVisible)
{
    Frustum f = makeFrustum();

    EXPECT_FALSE(isVisible(f, Sphere{ { 0.0f, 0.0f, -200.0f }, 1.0f })); // beyond far plane
    EXPECT_FALSE(isVisible(f, Sphere{ { 0.0f, 0.0f, 5.0f }, 1.0f }));    // behind camera
}

TEST(TestMath, FrustumAABBVisible)
{
    Frustum f = makeFrustum();

    // Small box around z=-5
    AABB inside{ { -0.5f, -0.5f, -5.5f }, { 0.5f, 0.5f, -4.5f } };
    EXPECT_TRUE(isVisible(f, inside));
}

TEST(TestMath, FrustumAABBNotVisible)
{
    Frustum f = makeFrustum();

    // Box entirely beyond far plane
    AABB beyond_far{ { -0.5f, -0.5f, -201.0f }, { 0.5f, 0.5f, -150.0f } };
    EXPECT_FALSE(isVisible(f, beyond_far));

    // Box entirely behind camera
    AABB behind{ { -0.5f, -0.5f, 5.0f }, { 0.5f, 0.5f, 10.0f } };
    EXPECT_FALSE(isVisible(f, behind));
}
