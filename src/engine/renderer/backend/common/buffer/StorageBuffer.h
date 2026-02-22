#ifndef RENDERER_BACKEND_COMMON_BUFFER_STORAGEBUFFER_H_
#define RENDERER_BACKEND_COMMON_BUFFER_STORAGEBUFFER_H_

#include <vector>

#include <volk.h>

#include "GpuBuffer.h"

// StorageBuffer is for storage buffers used with descriptor buffers (VK_EXT_descriptor_buffer).
// It automatically includes VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT required for
// vkGetBufferDeviceAddress() which is needed when writing to descriptor buffers.
class StorageBuffer : public GpuBuffer
{

public:

    StorageBuffer() = delete;
    StorageBuffer(const StorageBuffer& other) = delete;

    StorageBuffer(VkPhysicalDevice physical_device, VkDevice device, bool host_visible = true, bool enable_readback = false);

    ~StorageBuffer() override;

    StorageBuffer operator=(const StorageBuffer& other) = delete;

    bool create(VkDeviceSize size);

    template<typename T>
    bool create(const T& data);

    template<typename T>
    bool create(const std::vector<T>& data);

};

template<typename T>
bool StorageBuffer::create(const T& data)
{
    return GpuBuffer::create(data, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
}

template<typename T>
bool StorageBuffer::create(const std::vector<T>& data)
{
    return GpuBuffer::create(data, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
}

#endif /* RENDERER_BACKEND_COMMON_BUFFER_STORAGEBUFFER_H_ */
