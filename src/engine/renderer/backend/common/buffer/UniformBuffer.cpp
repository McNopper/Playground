#include "UniformBuffer.h"

UniformBuffer::UniformBuffer(VkPhysicalDevice physical_device, VkDevice device, bool host_visible, bool enable_readback) :
    GpuBuffer(physical_device, device, host_visible, enable_readback)
{
}

UniformBuffer::~UniformBuffer()
{
}

bool UniformBuffer::create(VkDeviceSize size)
{
    return GpuBuffer::create(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
}
