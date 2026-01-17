#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(VkPhysicalDevice physical_device, VkDevice device, bool host_visible, bool enable_readback) :
	GpuBuffer(physical_device, device, host_visible, enable_readback)
{
}

IndexBuffer::~IndexBuffer()
{
}

bool IndexBuffer::create(VkDeviceSize size)
{
	return GpuBuffer::create(size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}
