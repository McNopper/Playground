
#ifndef RENDERER_H_
#define RENDERER_H_

// Layer

// geometry

#include "engine/renderer/geometry/AGeometry.h"

#include "engine/renderer/geometry/point/APoint.h"

#include "engine/renderer/geometry/curve/ACurve.h"

#include "engine/renderer/geometry/surface/ASurface.h"
#include "engine/renderer/geometry/surface/TriangleMesh.h"

#include "engine/renderer/geometry/volume/AVolume.h"

// Layer

// backend/common

#include "engine/renderer/backend/common/buffer/GpuBuffer.h"
#include "engine/renderer/backend/common/buffer/VertexBuffer.h"
#include "engine/renderer/backend/common/buffer/IndexBuffer.h"
#include "engine/renderer/backend/common/buffer/UniformBuffer.h"
#include "engine/renderer/backend/common/buffer/StorageBuffer.h"
#include "engine/renderer/backend/common/buffer/DescriptorBuffer.h"
#include "engine/renderer/backend/common/buffer/DescriptorBufferSet.h"
#include "engine/renderer/backend/common/image/Texture.h"
#include "engine/renderer/backend/common/image/Texture2D.h"
#include "engine/renderer/backend/common/image/Sampler.h"

#endif /* RENDERER_H_ */
