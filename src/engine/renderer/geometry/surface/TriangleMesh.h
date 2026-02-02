#ifndef RENDERER_GEOMETRY_SURFACE_TRIANGLEMESH_H_
#define RENDERER_GEOMETRY_SURFACE_TRIANGLEMESH_H_

#include <map>
#include <memory>
#include <string>

#include <volk.h>

#include "engine/renderer/geometry/surface/ASurface.h"

// Forward declarations
class VertexBuffer;
class IndexBuffer;

// Triangle mesh geometry with named vertex attributes
// Supports both interleaved and separate vertex buffers
// Attribute names match shader reflection (e.g., "inPosition", "inNormal", "inTexCoord")
class TriangleMesh : public ASurface
{

private:

	VkPhysicalDevice m_physical_device{ VK_NULL_HANDLE };
	VkDevice m_device{ VK_NULL_HANDLE };

	// Vertex buffer metadata for each named attribute
	struct VertexAttribute
	{
		std::shared_ptr<VertexBuffer> buffer{};  // Pointer to vertex buffer
		uint32_t binding{ 0u };            // Vulkan binding index
		uint32_t offset{ 0u };             // Offset in bytes within buffer (for interleaved)
		uint32_t stride{ 0u };             // Stride in bytes (0 = tightly packed)
		VkFormat format{ VK_FORMAT_UNDEFINED }; // Vulkan format (e.g., VK_FORMAT_R32G32B32_SFLOAT)
	};

	// Map attribute name to buffer info
	std::map<std::string, VertexAttribute> m_vertex_attributes{};

	// Optional index buffer
	std::shared_ptr<IndexBuffer> m_index_buffer{};
	VkIndexType m_index_type{ VK_INDEX_TYPE_UINT32 };
	uint32_t m_index_count{ 0u };

	uint32_t m_vertex_count{ 0u };

public:

	TriangleMesh() = delete;

	TriangleMesh(VkPhysicalDevice physical_device, VkDevice device);

	virtual ~TriangleMesh();

	TriangleMesh(const TriangleMesh& other) = delete;
	TriangleMesh& operator=(const TriangleMesh& other) = delete;

	// Set vertex attribute by name (for shader reflection matching)
	// name: Shader attribute name (e.g., "inPosition", "inNormal")
	// buffer: Vertex buffer containing the data
	// binding: Vulkan binding index
	// format: Vulkan format of the attribute
	// offset: Byte offset within buffer (for interleaved data)
	// stride: Byte stride between vertices (0 = tightly packed, use format size)
	void setVertexAttribute(
		const std::string& name,
		const std::shared_ptr<VertexBuffer>& buffer,
		uint32_t binding,
		VkFormat format,
		uint32_t offset = 0u,
		uint32_t stride = 0u
	);

	// Set index buffer
	void setIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer, VkIndexType index_type, uint32_t count);

	// Set vertex count (for non-indexed draws)
	void setVertexCount(uint32_t count);

	// Check if attribute exists
	bool hasAttribute(const std::string& name) const;

	// Get attribute info (returns nullptr if not found)
	const VertexAttribute* getAttribute(const std::string& name) const;

	// Get index buffer info
	std::shared_ptr<IndexBuffer> getIndexBuffer() const;
	VkIndexType getIndexType() const;
	uint32_t getIndexCount() const;
	uint32_t getVertexCount() const;

	// Bind vertex and index buffers for rendering
	void bind(VkCommandBuffer command_buffer) const;

	// Issue draw call
	void draw(VkCommandBuffer command_buffer) const;

	bool isValid() const;

};

#endif /* RENDERER_GEOMETRY_SURFACE_TRIANGLEMESH_H_ */
