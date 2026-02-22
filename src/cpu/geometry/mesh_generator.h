#ifndef CPU_GEOMETRY_MESH_GENERATOR_H_
#define CPU_GEOMETRY_MESH_GENERATOR_H_

#include <cstdint>

#include "cpu/geometry/MeshData.h"

// Procedural mesh generators returning raw CPU-side MeshData.
// Winding order: counter-clockwise (front face = outside, Y-up).

// ---- 3D primitives — flat (hard normals) --------------------------------

// Regular tetrahedron centred at the origin.
// edge_length: length of each of the four equilateral triangular faces' edges.
// Per-face (hard) normals; 12 vertices, 12 indices.
MeshData createTetrahedron(float edge_length = 1.0f);

// Triangular prism (wedge) centred at the origin.
// Isoceles triangular cross-section in the XY plane, extruded along Z.
// width:  full X extent of the triangular base.
// height: full Y extent (base edge to apex).
// depth:  full Z extent.
// Per-face (hard) normals; 18 vertices, 24 indices.
MeshData createWedge(float width = 1.0f, float height = 1.0f, float depth = 1.0f);

// Axis-aligned cuboid centred at the origin.
// width, height, depth: full extents along X, Y, Z respectively.
// Per-face (hard) normals; 24 vertices, 36 indices.
MeshData createCuboid(float width = 1.0f, float height = 1.0f, float depth = 1.0f);

// Axis-aligned cube centred at the origin (delegates to createCuboid).
// size: full side length (default 1 → vertices at ±0.5).
MeshData createCube(float size = 1.0f);

// Regular pyramid centred at the origin (apex at +Y, polygonal base at −Y).
// Delegates to createCone with sides as segments and stacks = 1.
// base_radius: circumradius of the base polygon.
// height:      apex-to-base distance.
// sides:       number of base polygon sides (default 4 = square pyramid; 3 = triangular).
MeshData createPyramid(float base_radius = 1.0f, float height = 2.0f, uint32_t sides = 4u);

// Blunted (truncated) pyramid centred at the origin.
// Delegates to createFrustum with sides as segments and stacks = 1.
// top_radius:    circumradius of the top polygon.
// bottom_radius: circumradius of the base polygon.
// height:        total height.
// sides:         number of polygon sides (default 4 = square; 3 = triangular).
MeshData createBluntedPyramid(float top_radius = 0.5f, float bottom_radius = 1.0f,
                              float height = 2.0f, uint32_t sides = 4u);

// ---- 3D primitives — curved (smooth normals) ----------------------------

// UV sphere (or partial sphere) centred at the origin.
// radius:    sphere radius.
// stacks:    total latitudinal subdivisions pole-to-pole (default 16).
// segments:  longitudinal subdivisions around the equator (default 32).
// stack_cap: stacks to generate from the north pole; 0 = all (full sphere).
//            stacks/2 → upper hemisphere (dome); any value < stacks → partial sphere.
//            UV.v is always normalised to the full sphere, so textures are consistent.
MeshData createSphere(float radius = 1.0f, uint32_t stacks = 16u, uint32_t segments = 32u, uint32_t stack_cap = 0u);

// Ellipsoid centred at the origin (generalisation of sphere with independent semi-axes).
// radius_x, radius_y, radius_z: semi-axis lengths along X, Y, Z.
// stacks:   latitudinal subdivisions (default 16).
// segments: longitudinal subdivisions (default 32).
MeshData createEllipsoid(float radius_x = 1.0f, float radius_y = 0.5f, float radius_z = 1.0f,
                         uint32_t stacks = 16u, uint32_t segments = 32u);

// Cone centred at the origin (tip at +Y, base disc at −Y).
// radius:   base circle radius.
// height:   tip-to-base distance.
// segments: subdivisions around the base circle (default 32).
// stacks:   lateral subdivisions from tip to base (default 1 = single apex triangle fan).
MeshData createCone(float radius = 1.0f, float height = 2.0f, uint32_t segments = 32u, uint32_t stacks = 1u);

// Frustum (truncated cone) centred at the origin (top cap at +Y, bottom cap at −Y).
// top_radius:    radius of the top cap (smaller end).
// bottom_radius: radius of the bottom cap (larger end).
// height:        total height.
// segments:      subdivisions around the circumference (default 32).
// stacks:        lateral subdivisions from top to bottom (default 1).
MeshData createFrustum(float top_radius = 0.5f, float bottom_radius = 1.0f, float height = 2.0f,
                       uint32_t segments = 32u, uint32_t stacks = 1u);

// Blunted (truncated) cone centred at the origin (top cap at +Y, bottom cap at −Y).
// Delegates to createFrustum with the given segments and stacks = 1.
// top_radius:    radius of the top cap (smaller end).
// bottom_radius: radius of the bottom cap (larger end).
// height:        total height.
// segments:      subdivisions around the circumference (default 32).
MeshData createBluntedCone(float top_radius = 0.5f, float bottom_radius = 1.0f,
                           float height = 2.0f, uint32_t segments = 32u);

// Cylinder centred at the origin (top cap at +Y, bottom cap at −Y).
// Delegates to createFrustum with equal top and bottom radii.
// radius:   cap circle radius.
// height:   total height.
// segments: subdivisions around the circumference (default 32).
MeshData createCylinder(float radius = 1.0f, float height = 2.0f, uint32_t segments = 32u);

// Capsule centred at the origin (Y-axis aligned).
// radius:     hemisphere radius.
// height:     cylindrical section height (distance between hemisphere centres).
// cap_stacks: latitudinal subdivisions per hemisphere (default 8).
// segments:   longitudinal subdivisions around the circumference (default 32).
// Total capsule extent: height + 2*radius. UV.v is proportional to Y-position.
MeshData createCapsule(float radius = 0.5f, float height = 1.0f, uint32_t cap_stacks = 8u, uint32_t segments = 32u);

// Torus centred at the origin lying in the XZ plane.
// major_radius:   distance from torus centre to tube centre.
// minor_radius:   radius of the tube.
// major_segments: subdivisions around the torus ring (default 32).
// minor_segments: subdivisions around the tube cross-section (default 16).
MeshData createTorus(
    float major_radius = 1.0f,
    float minor_radius = 0.25f,
    uint32_t major_segments = 32u,
    uint32_t minor_segments = 16u);

// ---- 2D primitives — flat -----------------------------------------------

// Axis-aligned plane centred at the origin lying in the XZ plane (normal +Y).
// width:   extent along X.
// depth:   extent along Z.
// columns: grid subdivisions along X (default 1 = single quad pair).
// rows:    grid subdivisions along Z (default 1 = single quad pair).
MeshData createPlane(float width = 1.0f, float depth = 1.0f, uint32_t columns = 1u, uint32_t rows = 1u);

// ---- 2D primitives — curved ---------------------------------------------

// Filled disc centred at the origin lying in the XZ plane (normal +Y).
// radius:   disc radius.
// segments: subdivisions around the circumference (default 32).
MeshData createDisc(float radius = 1.0f, uint32_t segments = 32u);

#endif /* CPU_GEOMETRY_MESH_GENERATOR_H_ */
