#include <algorithm>
#include <cmath>
#include <cstdint>
#include <ranges>

#include <gtest/gtest.h>

#include "cpu/cpu.h"

// Helpers

static float length3(const float3& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

static bool normalsUnit(const MeshData& mesh, float tolerance = 1e-5f)
{
    return std::ranges::all_of(mesh.normals, [tolerance](const float3& n) {
        return std::abs(length3(n) - 1.0f) <= tolerance;
    });
}

static bool tangentsUnit(const MeshData& mesh, float tolerance = 1e-5f)
{
    return std::ranges::all_of(mesh.tangents, [tolerance](const float3& t) {
        return std::abs(length3(t) - 1.0f) <= tolerance;
    });
}

static bool indicesInBounds(const MeshData& mesh)
{
    const auto vertex_count{ static_cast<uint32_t>(mesh.positions.size()) };
    return std::ranges::all_of(mesh.indices, [vertex_count](uint32_t idx) {
        return idx < vertex_count;
    });
}

static bool attributeCountsMatch(const MeshData& mesh)
{
    const auto n{ mesh.positions.size() };
    return mesh.normals.size() == n && mesh.tangents.size() == n && mesh.uvs.size() == n;
}

// ---- PrimitiveCube -------------------------------------------------------

TEST(MeshFactoryCube, VertexAndIndexCount)
{
    const auto mesh = createCube();
    EXPECT_EQ(mesh.positions.size(), 24u); // 6 faces × 4 vertices
    EXPECT_EQ(mesh.indices.size(), 36u);   // 6 faces × 2 triangles × 3 indices
}

TEST(MeshFactoryCube, AttributeCountsMatch)
{
    EXPECT_TRUE(attributeCountsMatch(createCube()));
}

TEST(MeshFactoryCube, NormalsAreUnit)
{
    EXPECT_TRUE(normalsUnit(createCube()));
}

TEST(MeshFactoryCube, TangentsAreUnit)
{
    EXPECT_TRUE(tangentsUnit(createCube()));
}

TEST(MeshFactoryCube, IndicesInBounds)
{
    EXPECT_TRUE(indicesInBounds(createCube()));
}

TEST(MeshFactoryCube, ScaleAffectsPositions)
{
    const auto m1 = createCube(1.0f);
    const auto m2 = createCube(2.0f);
    EXPECT_FLOAT_EQ(m2.positions[0].x, m1.positions[0].x * 2.0f);
}

// ---- MeshFactoryCuboid ---------------------------------------------------

TEST(MeshFactoryCuboid, VertexAndIndexCount)
{
    const auto mesh = createCuboid(1.0f, 2.0f, 3.0f);
    EXPECT_EQ(mesh.positions.size(), 24u);
    EXPECT_EQ(mesh.indices.size(), 36u);
}

TEST(MeshFactoryCuboid, AttributeCountsMatch)
{
    EXPECT_TRUE(attributeCountsMatch(createCuboid(1.0f, 2.0f, 3.0f)));
}

TEST(MeshFactoryCuboid, NormalsAreUnit)
{
    EXPECT_TRUE(normalsUnit(createCuboid(1.0f, 2.0f, 3.0f)));
}

TEST(MeshFactoryCuboid, TangentsAreUnit)
{
    EXPECT_TRUE(tangentsUnit(createCuboid(1.0f, 2.0f, 3.0f)));
}

TEST(MeshFactoryCuboid, IndicesInBounds)
{
    EXPECT_TRUE(indicesInBounds(createCuboid(1.0f, 2.0f, 3.0f)));
}

TEST(MeshFactoryCuboid, MatchesCubeWhenUniform)
{
    const auto cube = createCube(2.0f);
    const auto cuboid = createCuboid(2.0f, 2.0f, 2.0f);
    ASSERT_EQ(cube.positions.size(), cuboid.positions.size());
    for (std::size_t i{ 0u }; i < cube.positions.size(); ++i)
    {
        EXPECT_FLOAT_EQ(cube.positions[i].x, cuboid.positions[i].x);
        EXPECT_FLOAT_EQ(cube.positions[i].y, cuboid.positions[i].y);
        EXPECT_FLOAT_EQ(cube.positions[i].z, cuboid.positions[i].z);
    }
    EXPECT_EQ(cube.indices, cuboid.indices);
}

TEST(MeshFactoryCuboid, ExtentsMatchDimensions)
{
    const auto mesh = createCuboid(2.0f, 4.0f, 6.0f);
    const auto [xmin, xmax] = std::ranges::minmax(mesh.positions | std::views::transform([](const float3& p) { return p.x; }));
    const auto [ymin, ymax] = std::ranges::minmax(mesh.positions | std::views::transform([](const float3& p) { return p.y; }));
    const auto [zmin, zmax] = std::ranges::minmax(mesh.positions | std::views::transform([](const float3& p) { return p.z; }));
    EXPECT_FLOAT_EQ(xmax - xmin, 2.0f);
    EXPECT_FLOAT_EQ(ymax - ymin, 4.0f);
    EXPECT_FLOAT_EQ(zmax - zmin, 6.0f);
}

// ---- PrimitiveSphere -----------------------------------------------------

TEST(MeshFactorySphere, VertexAndIndexCount)
{
    constexpr uint32_t stacks{ 8u };
    constexpr uint32_t segs{ 16u };
    const auto mesh = createSphere(1.0f, stacks, segs);
    EXPECT_EQ(mesh.positions.size(), static_cast<std::size_t>((stacks + 1u) * (segs + 1u)));
    EXPECT_EQ(mesh.indices.size(), static_cast<std::size_t>(stacks * segs * 6u));
}

TEST(MeshFactorySphere, AttributeCountsMatch)
{
    EXPECT_TRUE(attributeCountsMatch(createSphere()));
}

TEST(MeshFactorySphere, NormalsAreUnit)
{
    EXPECT_TRUE(normalsUnit(createSphere()));
}

TEST(MeshFactorySphere, TangentsAreUnit)
{
    EXPECT_TRUE(tangentsUnit(createSphere()));
}

TEST(MeshFactorySphere, IndicesInBounds)
{
    EXPECT_TRUE(indicesInBounds(createSphere()));
}

TEST(MeshFactorySphere, DomeVertexAndIndexCount)
{
    // stack_cap = stacks/2 → upper hemisphere
    constexpr uint32_t stacks{ 16u };
    constexpr uint32_t segs{ 32u };
    const auto mesh = createSphere(1.0f, stacks, segs, stacks / 2u);
    EXPECT_EQ(mesh.positions.size(), static_cast<std::size_t>((stacks / 2u + 1u) * (segs + 1u)));
    EXPECT_EQ(mesh.indices.size(), static_cast<std::size_t>((stacks / 2u) * segs * 6u));
}

TEST(MeshFactorySphere, DomeTopVertexAtNorthPole)
{
    const auto mesh = createSphere(1.0f, 16u, 32u, 8u);
    // First vertex: north pole (0, radius, 0)
    EXPECT_NEAR(mesh.positions[0].x, 0.0f, 1e-5f);
    EXPECT_NEAR(mesh.positions[0].y, 1.0f, 1e-5f);
    EXPECT_NEAR(mesh.positions[0].z, 0.0f, 1e-5f);
}

TEST(MeshFactorySphere, DomeUVConsistentWithFullSphere)
{
    // UV.v at the dome rim (stack_cap = stacks/2) must equal 0.5 (equator)
    constexpr uint32_t stacks{ 16u };
    constexpr uint32_t segs{ 32u };
    const auto mesh = createSphere(1.0f, stacks, segs, stacks / 2u);
    // Last ring starts at index (stacks/2)*(segs+1)
    const float v_rim = mesh.uvs[(stacks / 2u) * (segs + 1u)].y;
    EXPECT_NEAR(v_rim, 0.5f, 1e-5f);
}

TEST(MeshFactorySphere, RadiusAffectsPositions)
{
    const auto mesh = createSphere(3.0f, 8u, 16u);
    for (const auto& p : mesh.positions)
    {
        EXPECT_NEAR(length3(p), 3.0f, 1e-5f);
    }
}

// ---- MeshFactoryEllipsoid ------------------------------------------------

TEST(MeshFactoryEllipsoid, VertexAndIndexCount)
{
    constexpr uint32_t st{ 8u };
    constexpr uint32_t sg{ 16u };
    const auto mesh = createEllipsoid(2.0f, 1.0f, 0.5f, st, sg);
    EXPECT_EQ(mesh.positions.size(), static_cast<std::size_t>((st + 1u) * (sg + 1u)));
    EXPECT_EQ(mesh.indices.size(), static_cast<std::size_t>(st * sg * 6u));
}

TEST(MeshFactoryEllipsoid, AttributeCountsMatch)
{
    EXPECT_TRUE(attributeCountsMatch(createEllipsoid()));
}

TEST(MeshFactoryEllipsoid, NormalsAreUnit)
{
    EXPECT_TRUE(normalsUnit(createEllipsoid()));
}

TEST(MeshFactoryEllipsoid, TangentsAreUnit)
{
    EXPECT_TRUE(tangentsUnit(createEllipsoid()));
}

TEST(MeshFactoryEllipsoid, IndicesInBounds)
{
    EXPECT_TRUE(indicesInBounds(createEllipsoid()));
}

TEST(MeshFactoryEllipsoid, SemiAxesAffectExtents)
{
    const auto mesh = createEllipsoid(3.0f, 2.0f, 1.0f, 16u, 32u);
    const float xmax = std::ranges::max(mesh.positions, {}, [](const float3& p) { return p.x; }).x;
    const float ymax = std::ranges::max(mesh.positions, {}, [](const float3& p) { return p.y; }).y;
    const float zmax = std::ranges::max(mesh.positions, {}, [](const float3& p) { return p.z; }).z;
    EXPECT_NEAR(xmax, 3.0f, 1e-4f);
    EXPECT_NEAR(ymax, 2.0f, 1e-4f);
    EXPECT_NEAR(zmax, 1.0f, 1e-4f);
}

// ---- MeshFactoryCapsule --------------------------------------------------

TEST(MeshFactoryCapsule, VertexAndIndexCount)
{
    constexpr uint32_t cs{ 8u };
    constexpr uint32_t segs{ 32u };
    const auto mesh = createCapsule(0.5f, 1.0f, cs, segs);
    // Rings: 2*cs+2;  strips: 2*cs+1
    EXPECT_EQ(mesh.positions.size(), static_cast<std::size_t>((2u * cs + 2u) * (segs + 1u)));
    EXPECT_EQ(mesh.indices.size(), static_cast<std::size_t>((2u * cs + 1u) * segs * 6u));
}

TEST(MeshFactoryCapsule, AttributeCountsMatch)
{
    EXPECT_TRUE(attributeCountsMatch(createCapsule()));
}

TEST(MeshFactoryCapsule, NormalsAreUnit)
{
    EXPECT_TRUE(normalsUnit(createCapsule()));
}

TEST(MeshFactoryCapsule, TangentsAreUnit)
{
    EXPECT_TRUE(tangentsUnit(createCapsule()));
}

TEST(MeshFactoryCapsule, IndicesInBounds)
{
    EXPECT_TRUE(indicesInBounds(createCapsule()));
}

TEST(MeshFactoryCapsule, PolePositions)
{
    // radius=0.5, height=1.0 → north pole y=1.0, south pole y=-1.0
    const auto mesh = createCapsule(0.5f, 1.0f, 8u, 32u);
    EXPECT_NEAR(mesh.positions.front().y, 1.0f, 1e-5f); // north pole
    EXPECT_NEAR(mesh.positions.back().y, -1.0f, 1e-5f); // south pole
}

TEST(MeshFactoryCapsule, UVRangeZeroToOne)
{
    const auto mesh = createCapsule();
    const float v_min = std::ranges::min(mesh.uvs, {}, [](const float2& uv) { return uv.y; }).y;
    const float v_max = std::ranges::max(mesh.uvs, {}, [](const float2& uv) { return uv.y; }).y;
    EXPECT_NEAR(v_min, 0.0f, 1e-5f);
    EXPECT_NEAR(v_max, 1.0f, 1e-5f);
}

// ---- PrimitiveCone -------------------------------------------------------

TEST(MeshFactoryCone, VertexAndIndexCount)
{
    constexpr uint32_t segs{ 16u };
    const auto mesh = createCone(1.0f, 2.0f, segs);
    // Side: 2 rings (apex + base) × (segs+1) vertices; cap: segs × 3 (triangle fan)
    EXPECT_EQ(mesh.positions.size(), static_cast<std::size_t>(2u * (segs + 1u) + segs * 3u));
    // Apex strip triangles + cap triangles
    EXPECT_EQ(mesh.indices.size(), static_cast<std::size_t>(segs * 6u));
}

TEST(MeshFactoryCone, MultiStackVertexAndIndexCount)
{
    constexpr uint32_t segs{ 16u };
    constexpr uint32_t stacks{ 4u };
    const auto mesh = createCone(1.0f, 2.0f, segs, stacks);
    EXPECT_EQ(mesh.positions.size(), static_cast<std::size_t>((stacks + 1u) * (segs + 1u) + segs * 3u));
    EXPECT_EQ(mesh.indices.size(), static_cast<std::size_t>(segs * 6u * stacks));
}

TEST(MeshFactoryCone, AttributeCountsMatch)
{
    EXPECT_TRUE(attributeCountsMatch(createCone()));
}

TEST(MeshFactoryCone, NormalsAreUnit)
{
    EXPECT_TRUE(normalsUnit(createCone()));
}

TEST(MeshFactoryCone, TangentsAreUnit)
{
    EXPECT_TRUE(tangentsUnit(createCone()));
}

TEST(MeshFactoryCone, IndicesInBounds)
{
    EXPECT_TRUE(indicesInBounds(createCone()));
}

// ---- MeshFactoryPyramid --------------------------------------------------

TEST(MeshFactoryPyramid, VertexAndIndexCount)
{
    // sides=4: side verts = 2*(4+1)=10, cap verts = 4*3=12 → 22 verts; indices = 4*6=24
    constexpr uint32_t sides{ 4u };
    const auto mesh = createPyramid(1.0f, 2.0f, sides);
    EXPECT_EQ(mesh.positions.size(), static_cast<std::size_t>(2u * (sides + 1u) + sides * 3u));
    EXPECT_EQ(mesh.indices.size(), static_cast<std::size_t>(sides * 6u));
}

TEST(MeshFactoryPyramid, AttributeCountsMatch)
{
    EXPECT_TRUE(attributeCountsMatch(createPyramid()));
}

TEST(MeshFactoryPyramid, NormalsAreUnit)
{
    EXPECT_TRUE(normalsUnit(createPyramid()));
}

TEST(MeshFactoryPyramid, TangentsAreUnit)
{
    EXPECT_TRUE(tangentsUnit(createPyramid()));
}

TEST(MeshFactoryPyramid, IndicesInBounds)
{
    EXPECT_TRUE(indicesInBounds(createPyramid()));
}

TEST(MeshFactoryPyramid, TriangularPyramid)
{
    // sides=3: 2*(3+1)+3*3=17 verts, 3*6=18 indices
    const auto mesh = createPyramid(1.0f, 2.0f, 3u);
    EXPECT_EQ(mesh.positions.size(), 17u);
    EXPECT_EQ(mesh.indices.size(), 18u);
}

TEST(MeshFactoryPyramid, MatchesConeWithSameSides)
{
    constexpr uint32_t sides{ 5u };
    const auto pyramid = createPyramid(1.0f, 2.0f, sides);
    const auto cone = createCone(1.0f, 2.0f, sides, 1u);
    EXPECT_EQ(pyramid.positions.size(), cone.positions.size());
    EXPECT_EQ(pyramid.indices.size(), cone.indices.size());
}

// ---- MeshFactoryFrustum --------------------------------------------------

TEST(MeshFactoryFrustum, VertexAndIndexCount)
{
    // stacks=2, segs=8: side=(2+1)*(8+1)=27; caps=2*8*3=48; total=75
    // indices: 2*8*6 + 2*8*3 = 96+48 = 144 = 6*8*(2+1)
    constexpr uint32_t st{ 2u };
    constexpr uint32_t sg{ 8u };
    const auto mesh = createFrustum(0.5f, 1.0f, 2.0f, sg, st);
    EXPECT_EQ(mesh.positions.size(), static_cast<std::size_t>((st + 1u) * (sg + 1u) + 2u * sg * 3u));
    EXPECT_EQ(mesh.indices.size(), static_cast<std::size_t>(6u * sg * (st + 1u)));
}

TEST(MeshFactoryFrustum, AttributeCountsMatch)
{
    EXPECT_TRUE(attributeCountsMatch(createFrustum()));
}

TEST(MeshFactoryFrustum, NormalsAreUnit)
{
    EXPECT_TRUE(normalsUnit(createFrustum()));
}

TEST(MeshFactoryFrustum, TangentsAreUnit)
{
    EXPECT_TRUE(tangentsUnit(createFrustum()));
}

TEST(MeshFactoryFrustum, IndicesInBounds)
{
    EXPECT_TRUE(indicesInBounds(createFrustum()));
}

TEST(MeshFactoryFrustum, MatchesCylinderWhenEqualRadii)
{
    constexpr uint32_t segs{ 12u };
    const auto frustum = createFrustum(1.0f, 1.0f, 2.0f, segs, 1u);
    const auto cylinder = createCylinder(1.0f, 2.0f, segs);
    EXPECT_EQ(frustum.positions.size(), cylinder.positions.size());
    EXPECT_EQ(frustum.indices.size(), cylinder.indices.size());
}

// ---- MeshFactoryBluntedPyramid -------------------------------------------

TEST(MeshFactoryBluntedPyramid, AttributeCountsMatch)
{
    EXPECT_TRUE(attributeCountsMatch(createBluntedPyramid()));
}

TEST(MeshFactoryBluntedPyramid, NormalsAreUnit)
{
    EXPECT_TRUE(normalsUnit(createBluntedPyramid()));
}

TEST(MeshFactoryBluntedPyramid, IndicesInBounds)
{
    EXPECT_TRUE(indicesInBounds(createBluntedPyramid()));
}

TEST(MeshFactoryBluntedPyramid, MatchesFrustumWithSameSides)
{
    constexpr uint32_t sides{ 6u };
    const auto bp = createBluntedPyramid(0.5f, 1.0f, 2.0f, sides);
    const auto frustum = createFrustum(0.5f, 1.0f, 2.0f, sides, 1u);
    EXPECT_EQ(bp.positions.size(), frustum.positions.size());
    EXPECT_EQ(bp.indices.size(), frustum.indices.size());
}

// ---- MeshFactoryBluntedCone -----------------------------------------------

TEST(MeshFactoryBluntedCone, AttributeCountsMatch)
{
    EXPECT_TRUE(attributeCountsMatch(createBluntedCone()));
}

TEST(MeshFactoryBluntedCone, NormalsAreUnit)
{
    EXPECT_TRUE(normalsUnit(createBluntedCone()));
}

TEST(MeshFactoryBluntedCone, IndicesInBounds)
{
    EXPECT_TRUE(indicesInBounds(createBluntedCone()));
}

TEST(MeshFactoryBluntedCone, MatchesFrustumWithSameSegments)
{
    constexpr uint32_t segments{ 16u };
    const auto bc = createBluntedCone(0.5f, 1.0f, 2.0f, segments);
    const auto frustum = createFrustum(0.5f, 1.0f, 2.0f, segments, 1u);
    EXPECT_EQ(bc.positions.size(), frustum.positions.size());
    EXPECT_EQ(bc.indices.size(), frustum.indices.size());
}

TEST(MeshFactoryTorus, VertexAndIndexCount)
{
    constexpr uint32_t maj{ 16u };
    constexpr uint32_t min{ 8u };
    const auto mesh = createTorus(1.0f, 0.25f, maj, min);
    EXPECT_EQ(mesh.positions.size(), static_cast<std::size_t>((min + 1u) * (maj + 1u)));
    EXPECT_EQ(mesh.indices.size(), static_cast<std::size_t>(min * maj * 6u));
}

TEST(MeshFactoryTorus, AttributeCountsMatch)
{
    EXPECT_TRUE(attributeCountsMatch(createTorus()));
}

TEST(MeshFactoryTorus, NormalsAreUnit)
{
    EXPECT_TRUE(normalsUnit(createTorus()));
}

TEST(MeshFactoryTorus, TangentsAreUnit)
{
    EXPECT_TRUE(tangentsUnit(createTorus()));
}

TEST(MeshFactoryTorus, IndicesInBounds)
{
    EXPECT_TRUE(indicesInBounds(createTorus()));
} // ---- MeshFactoryCylinder -------------------------------------------------

TEST(MeshFactoryCylinder, VertexAndIndexCount)
{
    constexpr uint32_t segs{ 16u };
    const auto mesh = createCylinder(1.0f, 2.0f, segs);
    // Lateral: 2*(segs+1); top cap + bottom cap: 2*(segs*3)
    EXPECT_EQ(mesh.positions.size(), static_cast<std::size_t>(2u * (segs + 1u) + 2u * segs * 3u));
    // Lateral quads + top cap + bottom cap
    EXPECT_EQ(mesh.indices.size(), static_cast<std::size_t>(segs * 6u + 2u * segs * 3u));
}

TEST(MeshFactoryCylinder, AttributeCountsMatch)
{
    EXPECT_TRUE(attributeCountsMatch(createCylinder()));
}

TEST(MeshFactoryCylinder, NormalsAreUnit)
{
    EXPECT_TRUE(normalsUnit(createCylinder()));
}

TEST(MeshFactoryCylinder, TangentsAreUnit)
{
    EXPECT_TRUE(tangentsUnit(createCylinder()));
}

TEST(MeshFactoryCylinder, IndicesInBounds)
{
    EXPECT_TRUE(indicesInBounds(createCylinder()));
}

// ---- MeshFactoryWedge ----------------------------------------------------

TEST(MeshFactoryWedge, VertexAndIndexCount)
{
    const auto mesh = createWedge();
    EXPECT_EQ(mesh.positions.size(), 24u); // 5 faces, no shared verts: 2×3 + 3×(2×3)
    EXPECT_EQ(mesh.indices.size(), 24u);   // 8 triangles × 3
}

TEST(MeshFactoryWedge, AttributeCountsMatch)
{
    EXPECT_TRUE(attributeCountsMatch(createWedge()));
}

TEST(MeshFactoryWedge, NormalsAreUnit)
{
    EXPECT_TRUE(normalsUnit(createWedge()));
}

TEST(MeshFactoryWedge, TangentsAreUnit)
{
    EXPECT_TRUE(tangentsUnit(createWedge()));
}

TEST(MeshFactoryWedge, IndicesInBounds)
{
    EXPECT_TRUE(indicesInBounds(createWedge()));
}

TEST(MeshFactoryWedge, ExtentsMatchDimensions)
{
    const auto mesh = createWedge(4.0f, 2.0f, 6.0f);
    const auto [xmin, xmax] = std::ranges::minmax(mesh.positions | std::views::transform([](const float3& p) { return p.x; }));
    const auto [ymin, ymax] = std::ranges::minmax(mesh.positions | std::views::transform([](const float3& p) { return p.y; }));
    const auto [zmin, zmax] = std::ranges::minmax(mesh.positions | std::views::transform([](const float3& p) { return p.z; }));
    EXPECT_FLOAT_EQ(xmax - xmin, 4.0f);
    EXPECT_FLOAT_EQ(ymax - ymin, 2.0f);
    EXPECT_FLOAT_EQ(zmax - zmin, 6.0f);
}

// ---- MeshFactoryTetrahedron ----------------------------------------------

TEST(MeshFactoryTetrahedron, VertexAndIndexCount)
{
    const auto mesh = createTetrahedron();
    EXPECT_EQ(mesh.positions.size(), 12u); // 4 faces × 3 vertices
    EXPECT_EQ(mesh.indices.size(), 12u);   // 4 triangles × 3
}

TEST(MeshFactoryTetrahedron, AttributeCountsMatch)
{
    EXPECT_TRUE(attributeCountsMatch(createTetrahedron()));
}

TEST(MeshFactoryTetrahedron, NormalsAreUnit)
{
    EXPECT_TRUE(normalsUnit(createTetrahedron()));
}

TEST(MeshFactoryTetrahedron, TangentsAreUnit)
{
    EXPECT_TRUE(tangentsUnit(createTetrahedron()));
}

TEST(MeshFactoryTetrahedron, IndicesInBounds)
{
    EXPECT_TRUE(indicesInBounds(createTetrahedron()));
}

TEST(MeshFactoryTetrahedron, EdgeLengthIsCorrect)
{
    constexpr float edge{ 2.0f };
    const auto mesh = createTetrahedron(edge);
    // positions[0]=V0, positions[1]=V1 — two adjacent vertices on the same face.
    const float3& a{ mesh.positions[0] };
    const float3& b{ mesh.positions[1] };
    const float dx{ b.x - a.x };
    const float dy{ b.y - a.y };
    const float dz{ b.z - a.z };
    EXPECT_NEAR(std::sqrt(dx * dx + dy * dy + dz * dz), edge, 1e-5f);
}

// ---- MeshFactoryDisc -----------------------------------------------------

TEST(MeshFactoryDisc, VertexAndIndexCount)
{
    constexpr uint32_t segs{ 16u };
    const auto mesh = createDisc(1.0f, segs);
    // Centre + (segs+1) ring
    EXPECT_EQ(mesh.positions.size(), static_cast<std::size_t>(segs + 2u));
    EXPECT_EQ(mesh.indices.size(), static_cast<std::size_t>(segs * 3u));
}

TEST(MeshFactoryDisc, AttributeCountsMatch)
{
    EXPECT_TRUE(attributeCountsMatch(createDisc()));
}

TEST(MeshFactoryDisc, NormalsAreUnit)
{
    EXPECT_TRUE(normalsUnit(createDisc()));
}

TEST(MeshFactoryDisc, TangentsAreUnit)
{
    EXPECT_TRUE(tangentsUnit(createDisc()));
}

TEST(MeshFactoryDisc, IndicesInBounds)
{
    EXPECT_TRUE(indicesInBounds(createDisc()));
}

// ---- MeshFactoryPlane ----------------------------------------------------

TEST(MeshFactoryPlane, VertexAndIndexCount)
{
    constexpr uint32_t cols{ 4u };
    constexpr uint32_t rows_{ 4u };
    const auto mesh = createPlane(1.0f, 1.0f, cols, rows_);
    EXPECT_EQ(mesh.positions.size(), static_cast<std::size_t>((cols + 1u) * (rows_ + 1u)));
    EXPECT_EQ(mesh.indices.size(), static_cast<std::size_t>(cols * rows_ * 6u));
}

TEST(MeshFactoryPlane, AttributeCountsMatch)
{
    EXPECT_TRUE(attributeCountsMatch(createPlane()));
}

TEST(MeshFactoryPlane, NormalsAreUnit)
{
    EXPECT_TRUE(normalsUnit(createPlane()));
}

TEST(MeshFactoryPlane, TangentsAreUnit)
{
    EXPECT_TRUE(tangentsUnit(createPlane()));
}

TEST(MeshFactoryPlane, IndicesInBounds)
{
    EXPECT_TRUE(indicesInBounds(createPlane()));
}

TEST(MeshFactoryPlane, SizeAffectsPositions)
{
    const auto mesh = createPlane(4.0f, 2.0f, 1u, 1u);
    // X extents: ±2, Z extents: ±1
    const float max_x = std::ranges::max(mesh.positions, {}, [](const float3& p) { return p.x; }).x;
    const float max_z = std::ranges::max(mesh.positions, {}, [](const float3& p) { return p.z; }).z;
    EXPECT_FLOAT_EQ(max_x, 2.0f);
    EXPECT_FLOAT_EQ(max_z, 1.0f);
}
