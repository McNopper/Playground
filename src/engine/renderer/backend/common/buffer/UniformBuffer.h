#ifndef ENGINE_RENDERER_BACKEND_COMMON_BUFFER_UNIFORMBUFFER_H_
#define ENGINE_RENDERER_BACKEND_COMMON_BUFFER_UNIFORMBUFFER_H_

#include <vector>

#include <volk.h>

#include "GpuBuffer.h"

// UniformBuffer is for uniform buffers used with descriptor buffers (VK_EXT_descriptor_buffer).
// It automatically includes VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT required for
// vkGetBufferDeviceAddress() which is needed when writing to descriptor buffers.
class UniformBuffer : public GpuBuffer
{

public:

    UniformBuffer() = delete;
    UniformBuffer(const UniformBuffer& other) = delete;

    UniformBuffer(VkPhysicalDevice physical_device, VkDevice device, bool host_visible = true, bool enable_readback = false);

    ~UniformBuffer() override;

    UniformBuffer operator=(const UniformBuffer& other) = delete;

    bool create(VkDeviceSize size);

    template<typename T>
    bool create(const T& data);

    template<typename T>
    bool create(const std::vector<T>& data);
};

template<typename T>
bool UniformBuffer::create(const T& data)
{
    return GpuBuffer::create(data, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
}

template<typename T>
bool UniformBuffer::create(const std::vector<T>& data)
{
    return GpuBuffer::create(data, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
}

#endif /* ENGINE_RENDERER_BACKEND_COMMON_BUFFER_UNIFORMBUFFER_H_ */
