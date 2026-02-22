#include "cpu/geometry/mesh_generator.h"

#include <array>
#include <cmath>
#include <numbers>
#include <utility>

// ---------------------------------------------------------------------------
// Internal helpers (file-local)
// ---------------------------------------------------------------------------

// Pre-allocates all attribute vectors and the index vector in one call.
static void reserveMeshData(MeshData& mesh, uint32_t vertex_count, uint32_t index_count)
{
    mesh.positions.reserve(vertex_count);
    mesh.normals.reserve(vertex_count);
    mesh.tangents.reserve(vertex_count);
    mesh.uvs.reserve(vertex_count);
    mesh.indices.reserve(index_count);
}

// Emits one ring of (segments+1) vertices.
// Position: (r_k·cos, y_k, r_k·sin).  Normal: (n_xz·cos, n_y, n_xz·sin).
// Tangent:  (−sin, 0, cos).            UV: (j/segments, v).
static void appendRing(MeshData& mesh, float r_k, float y_k,
    float n_xz, float n_y, float v, uint32_t segments)
{
    const float seg_f{ static_cast<float>(segments) };
    for (uint32_t j{ 0u }; j <= segments; ++j)
    {
        const float theta{ 2.0f * std::numbers::pi_v<float> * static_cast<float>(j) / seg_f };
        const float cos_j{ std::cos(theta) };
        const float sin_j{ std::sin(theta) };

        mesh.positions.push_back({ r_k * cos_j, y_k, r_k * sin_j });
        mesh.normals.push_back({ n_xz * cos_j, n_y, n_xz * sin_j });
        mesh.tangents.push_back({ -sin_j, 0.0f, cos_j });
        mesh.uvs.push_back({ static_cast<float>(j) / seg_f, v });
    }
}

// Emits quad-strip indices connecting `strips` adjacent rings of (segments+1) vertices.
// vertex_base:   index of the first vertex of strip row 0 (default 0).
// flip_winding:  false = CCW front face (default), true = CW front face.
static void appendQuadStripIndices(MeshData& mesh, uint32_t strips,
    uint32_t segments, uint32_t vertex_base = 0u, bool flip_winding = false)
{
    for (uint32_t i{ 0u }; i < strips; ++i)
    {
        for (uint32_t j{ 0u }; j < segments; ++j)
        {
            const uint32_t a{ vertex_base + i * (segments + 1u) + j };
            const uint32_t b{ vertex_base + (i + 1u) * (segments + 1u) + j };

            if (flip_winding)
            {
                mesh.indices.push_back(a);
                mesh.indices.push_back(b + 1u);
                mesh.indices.push_back(a + 1u);

                mesh.indices.push_back(a);
                mesh.indices.push_back(b);
                mesh.indices.push_back(b + 1u);
            }
            else
            {
                mesh.indices.push_back(a);
                mesh.indices.push_back(a + 1u);
                mesh.indices.push_back(b + 1u);

                mesh.indices.push_back(a);
                mesh.indices.push_back(b + 1u);
                mesh.indices.push_back(b);
            }
        }
    }
}

// Computes the outward side-normal components for a surface of revolution whose
// radius changes by `delta_radius` over `height`.
// Returns {n_xz, n_y} where n_xz is the radial and n_y the axial component.
static std::pair<float, float> computeSlopeNormal(float delta_radius, float height)
{
    const float slope_len{ std::sqrt(delta_radius * delta_radius + height * height) };
    return { height / slope_len, delta_radius / slope_len };
}

// Emits (stacks+1) rings of vertices for a surface of revolution whose radius
// linearly interpolates from top_radius to bottom_radius over the given height.
// The shape is centred at the origin (top at +half_h, bottom at −half_h).
static void appendRevolutionRings(MeshData& mesh, float top_radius, float bottom_radius,
    float height, float n_xz, float n_y, uint32_t stacks, uint32_t segments)
{
    const float half_h{ height * 0.5f };
    const float dr{ bottom_radius - top_radius };

    for (uint32_t k{ 0u }; k <= stacks; ++k)
    {
        const float t{ static_cast<float>(k) / static_cast<float>(stacks) };
        appendRing(mesh, top_radius + t * dr, half_h - t * height, n_xz, n_y, t, segments);
    }
}

// Emits a flat circular cap (triangle fan) at height y.
// facing_up = true  → normal +Y, CCW from above: (centre, ring_{j+1}, ring_j).
// facing_up = false → normal −Y, CCW from below: (centre, ring_j,    ring_{j+1}).
static void appendCircularCap(MeshData& mesh, float y, float radius,
    const float3& normal, bool facing_up, uint32_t segments)
{
    const float seg_f{ static_cast<float>(segments) };
    for (uint32_t j{ 0u }; j < segments; ++j)
    {
        const float theta_j{ 2.0f * std::numbers::pi_v<float> * static_cast<float>(j) / seg_f };
        const float theta_k{ 2.0f * std::numbers::pi_v<float> * static_cast<float>(j + 1u) / seg_f };

        const float cos_j{ std::cos(theta_j) }; const float sin_j{ std::sin(theta_j) };
        const float cos_k{ std::cos(theta_k) }; const float sin_k{ std::sin(theta_k) };

        // Vertex A: ring_{j+1} for +Y cap, ring_j for −Y cap
        const float cos_a{ facing_up ? cos_k : cos_j };
        const float sin_a{ facing_up ? sin_k : sin_j };
        // Vertex B: ring_j for +Y cap, ring_{j+1} for −Y cap
        const float cos_b{ facing_up ? cos_j : cos_k };
        const float sin_b{ facing_up ? sin_j : sin_k };

        const uint32_t base{ static_cast<uint32_t>(mesh.positions.size()) };

        mesh.positions.push_back({ 0.0f, y, 0.0f });
        mesh.normals.push_back(normal);
        mesh.tangents.push_back({ 1.0f, 0.0f, 0.0f });
        mesh.uvs.push_back({ 0.5f, 0.5f });

        mesh.positions.push_back({ radius * cos_a, y, radius * sin_a });
        mesh.normals.push_back(normal);
        mesh.tangents.push_back({ -sin_a, 0.0f, cos_a });
        mesh.uvs.push_back({ 0.5f + 0.5f * cos_a, 0.5f + 0.5f * sin_a });

        mesh.positions.push_back({ radius * cos_b, y, radius * sin_b });
        mesh.normals.push_back(normal);
        mesh.tangents.push_back({ -sin_b, 0.0f, cos_b });
        mesh.uvs.push_back({ 0.5f + 0.5f * cos_b, 0.5f + 0.5f * sin_b });

        mesh.indices.push_back(base + 0u);
        mesh.indices.push_back(base + 1u);
        mesh.indices.push_back(base + 2u);
    }
}



// ---------------------------------------------------------------------------
// createCube
// ---------------------------------------------------------------------------

struct FaceDef
{
    float3 normal;
    float3 tangent;
    float3 bitangent;
};

MeshData createCuboid(float width, float height, float depth)
{
    const float half_w{ width  * 0.5f };
    const float half_h{ height * 0.5f };
    const float half_d{ depth  * 0.5f };

    // 6 faces: +X, -X, +Y, -Y, +Z, -Z
    // tangent = face-right, bitangent = face-up
    const std::array<FaceDef, 6> faces{{
        { { 1,  0,  0}, { 0,  0, -1}, {0, 1, 0} },  // +X
        { {-1,  0,  0}, { 0,  0,  1}, {0, 1, 0} },  // -X
        { { 0,  1,  0}, { 1,  0,  0}, {0, 0,-1} },  // +Y
        { { 0, -1,  0}, { 1,  0,  0}, {0, 0, 1} },  // -Y
        { { 0,  0,  1}, { 1,  0,  0}, {0, 1, 0} },  // +Z
        { { 0,  0, -1}, {-1,  0,  0}, {0, 1, 0} },  // -Z
    }};

    MeshData mesh{};
    reserveMeshData(mesh, 24u, 36u);

    for (const auto& face : faces)
    {
        const auto base{ static_cast<uint32_t>(mesh.positions.size()) };

        const float3 center{ face.normal.x * half_w, face.normal.y * half_h, face.normal.z * half_d };

        // Tangent/bitangent are axis-aligned unit vectors; dot with half-extents picks the right extent.
        const float th{ std::abs(face.tangent.x)   * half_w + std::abs(face.tangent.y)   * half_h + std::abs(face.tangent.z)   * half_d };
        const float bh{ std::abs(face.bitangent.x) * half_w + std::abs(face.bitangent.y) * half_h + std::abs(face.bitangent.z) * half_d };

        auto corner = [&](float ru, float bu) -> float3
        {
            return {
                center.x + ru * face.tangent.x * th + bu * face.bitangent.x * bh,
                center.y + ru * face.tangent.y * th + bu * face.bitangent.y * bh,
                center.z + ru * face.tangent.z * th + bu * face.bitangent.z * bh
            };
        };

        // 4 corners: bl(-1,-1), br(+1,-1), tr(+1,+1), tl(-1,+1)
        mesh.positions.push_back(corner(-1.0f, -1.0f));
        mesh.positions.push_back(corner( 1.0f, -1.0f));
        mesh.positions.push_back(corner( 1.0f,  1.0f));
        mesh.positions.push_back(corner(-1.0f,  1.0f));

        for (uint32_t i{ 0u }; i < 4u; ++i)
        {
            mesh.normals.push_back(face.normal);
            mesh.tangents.push_back(face.tangent);
        }

        mesh.uvs.push_back({ 0.0f, 1.0f });
        mesh.uvs.push_back({ 1.0f, 1.0f });
        mesh.uvs.push_back({ 1.0f, 0.0f });
        mesh.uvs.push_back({ 0.0f, 0.0f });

        // Two CCW triangles: bl-br-tr and bl-tr-tl
        mesh.indices.push_back(base + 0u);
        mesh.indices.push_back(base + 1u);
        mesh.indices.push_back(base + 2u);

        mesh.indices.push_back(base + 0u);
        mesh.indices.push_back(base + 2u);
        mesh.indices.push_back(base + 3u);
    }

    return mesh;
}

MeshData createCube(float size)
{
    return createCuboid(size, size, size);
}

// ---------------------------------------------------------------------------
// createEllipsoid
// ---------------------------------------------------------------------------

MeshData createEllipsoid(float radius_x, float radius_y, float radius_z,
                         uint32_t stacks, uint32_t segments)
{
    MeshData mesh{};
    reserveMeshData(mesh, (stacks + 1u) * (segments + 1u), stacks * segments * 6u);

    const float seg_f{ static_cast<float>(segments) };

    for (uint32_t i{ 0u }; i <= stacks; ++i)
    {
        const float phi{ std::numbers::pi_v<float> * static_cast<float>(i) / static_cast<float>(stacks) };
        const float sin_phi{ std::sin(phi) };
        const float cos_phi{ std::cos(phi) };

        for (uint32_t j{ 0u }; j <= segments; ++j)
        {
            const float theta{ 2.0f * std::numbers::pi_v<float> * static_cast<float>(j) / seg_f };
            const float cos_theta{ std::cos(theta) };
            const float sin_theta{ std::sin(theta) };

            mesh.positions.push_back({
                radius_x * sin_phi * cos_theta,
                radius_y * cos_phi,
                radius_z * sin_phi * sin_theta
            });

            // Normal = gradient of f = x²/rx² + y²/ry² + z²/rz², normalised.
            const float nx{ sin_phi * cos_theta / radius_x };
            const float ny{ cos_phi             / radius_y };
            const float nz{ sin_phi * sin_theta / radius_z };
            const float nlen{ std::sqrt(nx * nx + ny * ny + nz * nz) };
            mesh.normals.push_back({ nx / nlen, ny / nlen, nz / nlen });

            // Tangent = d(pos)/d(theta), normalised; degenerate at poles → use +X.
            const float tx{ -radius_x * sin_theta };
            const float tz{  radius_z * cos_theta };
            const float tlen{ std::sqrt(tx * tx + tz * tz) };
            if (tlen > 1e-6f)
            {
                mesh.tangents.push_back({ tx / tlen, 0.0f, tz / tlen });
            }
            else
            {
                mesh.tangents.push_back({ 1.0f, 0.0f, 0.0f });
            }

            mesh.uvs.push_back({
                static_cast<float>(j) / seg_f,
                static_cast<float>(i) / static_cast<float>(stacks)
            });
        }
    }

    appendQuadStripIndices(mesh, stacks, segments);

    return mesh;
}

// ---------------------------------------------------------------------------
// createSphere
// ---------------------------------------------------------------------------

MeshData createSphere(float radius, uint32_t stacks, uint32_t segments, uint32_t stack_cap)
{
    // active: how many stacks are actually generated (0 = all = full sphere)
    const uint32_t active{ (stack_cap == 0u || stack_cap > stacks) ? stacks : stack_cap };

    MeshData mesh{};
    reserveMeshData(mesh, (active + 1u) * (segments + 1u), active * segments * 6u);

    // phi: polar angle 0 (north pole) → pi * active/stacks
    // Divided by stacks (not active) so UV.v stays proportional to the full sphere.
    for (uint32_t i{ 0u }; i <= active; ++i)
    {
        const float phi{ std::numbers::pi_v<float> * static_cast<float>(i) / static_cast<float>(stacks) };
        const float sin_phi{ std::sin(phi) };
        const float cos_phi{ std::cos(phi) };
        appendRing(mesh, radius * sin_phi, radius * cos_phi, sin_phi, cos_phi,
                   static_cast<float>(i) / static_cast<float>(stacks), segments);
    }

    appendQuadStripIndices(mesh, active, segments);

    return mesh;
}

// ---------------------------------------------------------------------------
// createCone
// ---------------------------------------------------------------------------

MeshData createCone(float radius, float height, uint32_t segments, uint32_t stacks)
{
    // Side: (stacks+1) rings × (segments+1) vertices.
    // Apex strip (k=0): 1 triangle per segment (T1 degenerate, T2 only).
    // Ring strips (k=1..stacks-1): 2 triangles per segment.
    // Bottom cap: non-shared triangle fan.
    const float half_h{ height * 0.5f };
    const auto [n_xz, n_y] = computeSlopeNormal(radius, height);

    MeshData mesh{};
    reserveMeshData(mesh, (stacks + 1u) * (segments + 1u) + segments * 3u, segments * 6u * stacks);

    appendRevolutionRings(mesh, 0.0f, radius, height, n_xz, n_y, stacks, segments);

    // Apex strip (k=0): T1 is degenerate (ring 0 has r=0), emit T2 only
    for (uint32_t j{ 0u }; j < segments; ++j)
    {
        const uint32_t a{ j };
        const uint32_t b{ segments + 1u + j };
        mesh.indices.push_back(a);
        mesh.indices.push_back(b + 1u);
        mesh.indices.push_back(b);
    }

    // Ring strips (k=1..stacks-1): CCW quads from ring 1 onward
    if (stacks > 1u)
    {
        appendQuadStripIndices(mesh, stacks - 1u, segments, segments + 1u);
    }

    appendCircularCap(mesh, -half_h, radius, { 0.0f, -1.0f, 0.0f }, false, segments);

    return mesh;
}

// ---------------------------------------------------------------------------
// createPyramid
// ---------------------------------------------------------------------------

MeshData createPyramid(float base_radius, float height, uint32_t sides)
{
    return createCone(base_radius, height, sides, 1u);
}

// ---------------------------------------------------------------------------
// createFrustum  (truncated cone — generalises cylinder and cone)
// ---------------------------------------------------------------------------

MeshData createFrustum(float top_radius, float bottom_radius, float height,
                       uint32_t segments, uint32_t stacks)
{
    const float half_h{ height * 0.5f };
    const float dr{ bottom_radius - top_radius };
    const auto [n_xz, n_y] = computeSlopeNormal(dr, height);

    MeshData mesh{};
    reserveMeshData(mesh, (stacks + 1u) * (segments + 1u) + 2u * segments * 3u, 6u * segments * (stacks + 1u));

    appendRevolutionRings(mesh, top_radius, bottom_radius, height, n_xz, n_y, stacks, segments);

    appendQuadStripIndices(mesh, stacks, segments);
    appendCircularCap(mesh,  half_h, top_radius,    { 0.0f,  1.0f, 0.0f }, true,  segments);
    appendCircularCap(mesh, -half_h, bottom_radius, { 0.0f, -1.0f, 0.0f }, false, segments);

    return mesh;
}

// ---------------------------------------------------------------------------
// createBluntedPyramid
// ---------------------------------------------------------------------------

MeshData createBluntedPyramid(float top_radius, float bottom_radius,
                               float height, uint32_t sides)
{
    return createFrustum(top_radius, bottom_radius, height, sides, 1u);
}

// ---------------------------------------------------------------------------
// createBluntedCone
// ---------------------------------------------------------------------------

MeshData createBluntedCone(float top_radius, float bottom_radius,
                            float height, uint32_t segments)
{
    return createFrustum(top_radius, bottom_radius, height, segments, 1u);
}

// ---------------------------------------------------------------------------
// createCylinder
// ---------------------------------------------------------------------------

MeshData createCylinder(float radius, float height, uint32_t segments)
{
    return createFrustum(radius, radius, height, segments, 1u);
}

// ---------------------------------------------------------------------------
// createWedge  (triangular prism with hard per-face normals)
// ---------------------------------------------------------------------------

MeshData createWedge(float width, float height, float depth)
{
    const float hw{ width  * 0.5f };
    const float hh{ height * 0.5f };
    const float hd{ depth  * 0.5f };

    // 2 triangular caps × 3 verts + 3 rectangular faces × 4 verts = 18 verts
    // 2 × 1 tri + 3 × 2 tris = 8 tris = 24 indices
    MeshData mesh{};
    reserveMeshData(mesh, 18u, 24u);

    // Slant face normals: (-height, width/2, 0) / slant_len  and  (+height, width/2, 0) / slant_len
    const float slant_len{ std::sqrt(height * height + width * width * 0.25f) };
    const float3 n_left { -height / slant_len, (width * 0.5f) / slant_len, 0.0f };
    const float3 n_right{  height / slant_len, (width * 0.5f) / slant_len, 0.0f };

    // Emit a triangle with shared flat normal and tangent; advances position list.
    auto emitTri = [&](const float3& p0, const float3& p1, const float3& p2,
                       const float2& uv0, const float2& uv1, const float2& uv2,
                       const float3& n, const float3& t)
    {
        const uint32_t base{ static_cast<uint32_t>(mesh.positions.size()) };
        mesh.positions.push_back(p0); mesh.positions.push_back(p1); mesh.positions.push_back(p2);
        mesh.normals.push_back(n);    mesh.normals.push_back(n);    mesh.normals.push_back(n);
        mesh.tangents.push_back(t);   mesh.tangents.push_back(t);   mesh.tangents.push_back(t);
        mesh.uvs.push_back(uv0);      mesh.uvs.push_back(uv1);      mesh.uvs.push_back(uv2);
        mesh.indices.push_back(base); mesh.indices.push_back(base + 1u); mesh.indices.push_back(base + 2u);
    };

    // Emit a quad as two triangles: (p0,p1,p2) and (p0,p2,p3).
    auto emitQuad = [&](const float3& p0, const float3& p1, const float3& p2, const float3& p3,
                        const float2& uv0, const float2& uv1, const float2& uv2, const float2& uv3,
                        const float3& n, const float3& t)
    {
        emitTri(p0, p1, p2, uv0, uv1, uv2, n, t);
        emitTri(p0, p2, p3, uv0, uv2, uv3, n, t); // NOLINT(readability-suspicious-call-argument)
    };

    // Vertex positions reused across faces (all positions emitted per-face for hard normals):
    // A = front-bottom-left  (-hw,-hh,+hd)    B = front-bottom-right (+hw,-hh,+hd)
    // C = back-bottom-right  (+hw,-hh,-hd)    D = back-bottom-left   (-hw,-hh,-hd)
    // E = front-top-centre   (  0,+hh,+hd)    F = back-top-centre    (  0,+hh,-hd)

    // Front cap (+Z): A, B, E — CCW from +Z
    emitTri({-hw,-hh,+hd}, {+hw,-hh,+hd}, {0,+hh,+hd},
            {0,1},{1,1},{0.5f,0}, {0,0,1}, {1,0,0});

    // Back cap (−Z): D, F, C — CCW from −Z
    emitTri({-hw,-hh,-hd}, {0,+hh,-hd}, {+hw,-hh,-hd},
            {1,1},{0.5f,0},{0,1}, {0,0,-1}, {-1,0,0});

    // Bottom face (−Y): A, D, C, B — CCW from below; tangent = +X, U along width, V along depth
    emitQuad({-hw,-hh,+hd}, {-hw,-hh,-hd}, {+hw,-hh,-hd}, {+hw,-hh,+hd},
             {0,0},{0,1},{1,1},{1,0}, {0,-1,0}, {1,0,0});

    // Left face: A, E, F, D — CCW from left; tangent along depth (0,0,−1), U along depth, V along edge
    emitQuad({-hw,-hh,+hd}, {0,+hh,+hd}, {0,+hh,-hd}, {-hw,-hh,-hd},
             {0,1},{0,0},{1,0},{1,1}, n_left, {0,0,-1});

    // Right face: B, C, F, E — CCW from right; tangent along depth
    emitQuad({+hw,-hh,+hd}, {+hw,-hh,-hd}, {0,+hh,-hd}, {0,+hh,+hd},
             {0,1},{1,1},{1,0},{0,0}, n_right, {0,0,-1});

    return mesh;
}

// ---------------------------------------------------------------------------
// createTetrahedron  (regular, hard per-face normals)
// ---------------------------------------------------------------------------

MeshData createTetrahedron(float edge_length)
{
    // Place vertices at (±1,±1,±1) with an odd number of minus signs.
    // Edge between any two = 2√2; scale to requested edge_length.
    const float s{ edge_length / (2.0f * std::numbers::sqrt2_v<float>) };

    const std::array<float3, 4u> v{{
        { s,  s,  s},  // V0
        { s, -s, -s},  // V1
        {-s,  s, -s},  // V2
        {-s, -s,  s},  // V3
    }};

    // Face winding verified CCW from outside (cross-product outward from centroid):
    const std::array<std::array<uint32_t, 3u>, 4u> faces{{
        {0u, 1u, 2u},  // normal ( 1, 1,−1)/√3
        {0u, 3u, 1u},  // normal ( 1,−1, 1)/√3
        {0u, 2u, 3u},  // normal (−1, 1, 1)/√3
        {1u, 3u, 2u},  // normal (−1,−1,−1)/√3
    }};

    // UV: equilateral triangle layout — (0,1), (1,1), (0.5, 1−√3/2)
    const float uv_top_v{ 1.0f - std::numbers::sqrt3_v<float> * 0.5f };

    MeshData mesh{};
    reserveMeshData(mesh, 12u, 12u);

    for (const auto& fi : faces)
    {
        const float3& a{ v[fi[0]] };
        const float3& b{ v[fi[1]] };
        const float3& c{ v[fi[2]] };

        const float e1x{ b.x - a.x };
        const float e1y{ b.y - a.y };
        const float e1z{ b.z - a.z };
        const float e2x{ c.x - a.x };
        const float e2y{ c.y - a.y };
        const float e2z{ c.z - a.z };

        const float nx{ e1y * e2z - e1z * e2y };
        const float ny{ e1z * e2x - e1x * e2z };
        const float nz{ e1x * e2y - e1y * e2x };
        const float nlen{ std::sqrt(nx * nx + ny * ny + nz * nz) };
        const float3 normal{ nx / nlen, ny / nlen, nz / nlen };

        const float tlen{ std::sqrt(e1x * e1x + e1y * e1y + e1z * e1z) };
        const float3 tangent{ e1x / tlen, e1y / tlen, e1z / tlen };

        const uint32_t base{ static_cast<uint32_t>(mesh.positions.size()) };
        mesh.positions.push_back(a);
        mesh.positions.push_back(b);
        mesh.positions.push_back(c);
        mesh.normals.push_back(normal);  mesh.normals.push_back(normal);  mesh.normals.push_back(normal);
        mesh.tangents.push_back(tangent); mesh.tangents.push_back(tangent); mesh.tangents.push_back(tangent);
        mesh.uvs.push_back({ 0.0f, 1.0f });
        mesh.uvs.push_back({ 1.0f, 1.0f });
        mesh.uvs.push_back({ 0.5f, uv_top_v });
        mesh.indices.push_back(base);
        mesh.indices.push_back(base + 1u);
        mesh.indices.push_back(base + 2u);
    }

    return mesh;
}

// ---------------------------------------------------------------------------
// createDisc
// ---------------------------------------------------------------------------

MeshData createDisc(float radius, uint32_t segments)
{
    // Centre + (segments+1) ring vertices; wrap prevents a UV seam.
    const uint32_t vertex_count{ segments + 2u };
    const uint32_t index_count{ segments * 3u };

    MeshData mesh{};
    reserveMeshData(mesh, vertex_count, index_count);

    const float3 normal{ 0.0f, 1.0f, 0.0f };
    const float3 tangent{ 1.0f, 0.0f, 0.0f };
    const float seg_f{ static_cast<float>(segments) };

    // Centre (index 0)
    mesh.positions.push_back({ 0.0f, 0.0f, 0.0f });
    mesh.normals.push_back(normal);
    mesh.tangents.push_back(tangent);
    mesh.uvs.push_back({ 0.5f, 0.5f });

    // Ring vertices (indices 1 … segments+1)
    for (uint32_t j{ 0u }; j <= segments; ++j)
    {
        const float theta{ 2.0f * std::numbers::pi_v<float> * static_cast<float>(j) / seg_f };
        const float cos_j{ std::cos(theta) };
        const float sin_j{ std::sin(theta) };

        mesh.positions.push_back({ radius * cos_j, 0.0f, radius * sin_j });
        mesh.normals.push_back(normal);
        mesh.tangents.push_back(tangent);
        // Radial UV: U=1 at +X, V=1 at +Z (bottom of texture)
        mesh.uvs.push_back({ 0.5f + 0.5f * cos_j, 0.5f + 0.5f * sin_j });
    }

    // CCW from above (+Y): (centre, ring_{j+1}, ring_j)
    for (uint32_t j{ 0u }; j < segments; ++j)
    {
        mesh.indices.push_back(0u);
        mesh.indices.push_back(j + 2u);
        mesh.indices.push_back(j + 1u);
    }

    return mesh;
}

// ---------------------------------------------------------------------------
// createPlane
// ---------------------------------------------------------------------------

MeshData createPlane(float width, float depth, uint32_t columns, uint32_t rows)
{
    const uint32_t vertex_count{ (rows + 1u) * (columns + 1u) };
    const uint32_t index_count{ rows * columns * 6u };

    MeshData mesh{};
    reserveMeshData(mesh, vertex_count, index_count);

    const float3 normal{ 0.0f, 1.0f, 0.0f };
    const float3 tang{ 1.0f, 0.0f, 0.0f };
    const float half_w{ width * 0.5f };
    const float half_d{ depth * 0.5f };

    // Vertices: U increases with +X, V increases with +Z
    for (uint32_t row{ 0u }; row <= rows; ++row)
    {
        const float v{ static_cast<float>(row) / static_cast<float>(rows) };
        const float z{ -half_d + v * depth };

        for (uint32_t col{ 0u }; col <= columns; ++col)
        {
            const float u{ static_cast<float>(col) / static_cast<float>(columns) };
            const float x{ -half_w + u * width };

            mesh.positions.push_back({ x, 0.0f, z });
            mesh.normals.push_back(normal);
            mesh.tangents.push_back(tang);
            mesh.uvs.push_back({ u, v });
        }
    }

    // CCW from above (+Y): T1: (a, b, a+1), T2: (a+1, b, b+1)
    for (uint32_t row{ 0u }; row < rows; ++row)
    {
        for (uint32_t col{ 0u }; col < columns; ++col)
        {
            const uint32_t a{ row * (columns + 1u) + col };
            const uint32_t b{ (row + 1u) * (columns + 1u) + col };

            mesh.indices.push_back(a);
            mesh.indices.push_back(b);
            mesh.indices.push_back(a + 1u);

            mesh.indices.push_back(a + 1u);
            mesh.indices.push_back(b);
            mesh.indices.push_back(b + 1u);
        }
    }

    return mesh;
}

// ---------------------------------------------------------------------------
// createCapsule
// ---------------------------------------------------------------------------

MeshData createCapsule(float radius, float height, uint32_t cap_stacks, uint32_t segments)
{
    const float half_h{ height * 0.5f };
    const float total_h{ height + 2.0f * radius };
    const float pi_half{ std::numbers::pi_v<float> * 0.5f };

    // Ring layout (2*cap_stacks+2 rings, 2*cap_stacks+1 strips):
    //   k = 0..cap_stacks       : top hemisphere    (phi 0   → π/2, centred at +half_h)
    //   k = cap_stacks+1        : cylinder bottom   (phi π/2, centred at −half_h)
    //   k = cap_stacks+2..2cs+1 : bottom hemisphere (phi π/2 → π,   centred at −half_h)
    const uint32_t ring_count{ 2u * cap_stacks + 2u };
    const uint32_t strip_count{ 2u * cap_stacks + 1u };

    MeshData mesh{};
    reserveMeshData(mesh, ring_count * (segments + 1u), strip_count * segments * 6u);

    for (uint32_t k{ 0u }; k < ring_count; ++k)
    {
        float phi{};
        float centre_y{};

        if (k <= cap_stacks)
        {
            phi      = pi_half * static_cast<float>(k) / static_cast<float>(cap_stacks);
            centre_y = half_h;
        }
        else if (k == cap_stacks + 1u)
        {
            phi      = pi_half;
            centre_y = -half_h;
        }
        else
        {
            const uint32_t j{ k - (cap_stacks + 1u) };
            phi      = pi_half + pi_half * static_cast<float>(j) / static_cast<float>(cap_stacks);
            centre_y = -half_h;
        }

        const float cos_phi{ std::cos(phi) };
        const float sin_phi{ std::sin(phi) };
        const float y_k{ centre_y + radius * cos_phi };
        const float r_k{ radius * sin_phi };
        const float v{ (half_h + radius - y_k) / total_h };

        appendRing(mesh, r_k, y_k, sin_phi, cos_phi, v, segments);
    }

    appendQuadStripIndices(mesh, strip_count, segments);

    return mesh;
}

// ---------------------------------------------------------------------------
// createTorus
// ---------------------------------------------------------------------------

MeshData createTorus(float major_radius, float minor_radius, uint32_t major_segments, uint32_t minor_segments)
{
    const uint32_t vertex_count{ (minor_segments + 1u) * (major_segments + 1u) };
    const uint32_t index_count{ minor_segments * major_segments * 6u };

    MeshData mesh{};
    reserveMeshData(mesh, vertex_count, index_count);

    // theta: minor angle(tube cross-section), phi: major angle (ring)
    for (uint32_t i{ 0u }; i <= minor_segments; ++i)
    {
        const float theta{ 2.0f * std::numbers::pi_v<float> * static_cast<float>(i) / static_cast<float>(minor_segments) };
        const float cos_theta{ std::cos(theta) };
        const float sin_theta{ std::sin(theta) };

        for (uint32_t j{ 0u }; j <= major_segments; ++j)
        {
            const float phi{ 2.0f * std::numbers::pi_v<float> * static_cast<float>(j) / static_cast<float>(major_segments) };
            const float cos_phi{ std::cos(phi) };
            const float sin_phi{ std::sin(phi) };

            const float dist{ major_radius + minor_radius * cos_theta };

            mesh.positions.push_back({ dist * cos_phi, minor_radius * sin_theta, dist * sin_phi });
            // Outward normal: from tube centre toward surface point
            mesh.normals.push_back({ cos_theta * cos_phi, sin_theta, cos_theta * sin_phi });
            // Tangent = d(pos)/d(phi) normalised = (-sin_phi, 0, cos_phi)
            mesh.tangents.push_back({ -sin_phi, 0.0f, cos_phi });
            mesh.uvs.push_back({
                static_cast<float>(j) / static_cast<float>(major_segments),
                static_cast<float>(i) / static_cast<float>(minor_segments)
            });
        }
    }

    appendQuadStripIndices(mesh, minor_segments, major_segments, 0u, true);

    return mesh;
}

