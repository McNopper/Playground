#ifndef CPU_GEOMETRY_MESHDATA_H_
#define CPU_GEOMETRY_MESHDATA_H_

#include <cstdint>
#include <vector>

#include "core/math/vector.h"

// Raw CPU-side mesh data produced by procedural primitive generators.
// Intended to be uploaded to GPU buffers (e.g., via TriangleMesh).
struct MeshData
{
    std::vector<float3> positions{};
    std::vector<float3> normals{};
    std::vector<float3> tangents{};
    std::vector<float2> uvs{};
    std::vector<uint32_t> indices{};
};

#endif /* CPU_GEOMETRY_MESHDATA_H_ */
