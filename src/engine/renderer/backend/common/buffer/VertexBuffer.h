#ifndef RENDERER_BACKEND_COMMON_BUFFER_VERTEXBUFFER_H_
#define RENDERER_BACKEND_COMMON_BUFFER_VERTEXBUFFER_H_

#include <vector>

#include <volk.h>

#include "GpuBuffer.h"

class VertexBuffer : public GpuBuffer {

public:

	VertexBuffer() = delete;
	VertexBuffer(const VertexBuffer& other) = delete;

	VertexBuffer(VkPhysicalDevice physical_device, VkDevice device, bool host_visible = true, bool enable_readback = false);

	~VertexBuffer() override;

	VertexBuffer operator=(const VertexBuffer& other) = delete;

	bool create(VkDeviceSize size);

	template<typename T>
	bool create(const T& data);

	template<typename T>
	bool create(const std::vector<T>& data);

};

template<typename T>
bool VertexBuffer::create(const T& data)
{
	return GpuBuffer::create(data, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}

template<typename T>
bool VertexBuffer::create(const std::vector<T>& data)
{
	return GpuBuffer::create(data, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}

#endif /* RENDERER_BACKEND_COMMON_BUFFER_VERTEXBUFFER_H_ */
