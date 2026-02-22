#ifndef RENDERER_BACKEND_COMMON_BUFFER_INDEXBUFFER_H_
#define RENDERER_BACKEND_COMMON_BUFFER_INDEXBUFFER_H_

#include <vector>

#include <volk.h>

#include "GpuBuffer.h"

class IndexBuffer : public GpuBuffer
{

public:

    IndexBuffer() = delete;
    IndexBuffer(const IndexBuffer& other) = delete;

    IndexBuffer(VkPhysicalDevice physical_device, VkDevice device, bool host_visible = true, bool enable_readback = false);

    ~IndexBuffer() override;

    IndexBuffer operator=(const IndexBuffer& other) = delete;

    bool create(VkDeviceSize size);

    template<typename T>
    bool create(const T& data);

    template<typename T>
    bool create(const std::vector<T>& data);

};

template<typename T>
bool IndexBuffer::create(const T& data)
{
    return GpuBuffer::create(data, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}

template<typename T>
bool IndexBuffer::create(const std::vector<T>& data)
{
    return GpuBuffer::create(data, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}

#endif /* RENDERER_BACKEND_COMMON_BUFFER_INDEXBUFFER_H_ */
