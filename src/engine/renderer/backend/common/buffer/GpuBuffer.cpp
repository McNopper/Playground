#include "GpuBuffer.h"

#include "gpu/gpu.h"

GpuBuffer::GpuBuffer(VkPhysicalDevice physical_device, VkDevice device, bool host_visible, bool enable_readback) :
    m_physical_device{ physical_device }, m_device{ device }, m_buffer_resource{}, m_device_size{ 0u }, m_host_visible{ host_visible }, m_readback_enabled{ enable_readback }
{
}

GpuBuffer::~GpuBuffer()
{
    destroy();
}

bool GpuBuffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage)
{
    destroy();

    // Add TRANSFER_SRC_BIT if readback is enabled for DEVICE_LOCAL buffers
    if (!m_host_visible && m_readback_enabled)
    {
        usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    }

    VulkanBufferFactory buffer_factory{ m_device, size, usage };
    m_buffer_resource.buffer = buffer_factory.create();
    if (m_buffer_resource.buffer == VK_NULL_HANDLE)
    {
        return false;
    }

    VkMemoryAllocateFlags allocate_flags = (usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) ? VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT : 0;
    
    VkMemoryPropertyFlags memory_flags;
    if (m_host_visible)
    {
        memory_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }
    else
    {
        memory_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    }

    m_buffer_resource.device_memory = buildBufferDeviceMemory(m_physical_device, m_device, m_buffer_resource.buffer, memory_flags, allocate_flags);

    if (m_buffer_resource.device_memory == VK_NULL_HANDLE)
    {
        return false;
    }

    m_device_size = size;

    return true;
}

bool GpuBuffer::create(VkDeviceSize size, VkBufferUsageFlags usage)
{
    return createBuffer(size, usage);
}

VkBuffer GpuBuffer::getBuffer() const
{
    return m_buffer_resource.buffer;
}

VkDeviceSize GpuBuffer::getDeviceSize() const
{
    return m_device_size;
}

bool GpuBuffer::isValid() const
{
    return m_buffer_resource.buffer != VK_NULL_HANDLE && m_buffer_resource.device_memory != VK_NULL_HANDLE;
}

bool GpuBuffer::isHostVisible() const
{
    return m_host_visible;
}

bool GpuBuffer::isReadbackEnabled() const
{
    return m_readback_enabled;
}

void GpuBuffer::destroy()
{
    destroyResource(m_device, m_buffer_resource);

    m_device_size = 0u;
    // Note: m_readback_enabled is NOT reset here - it's a configuration setting, not buffer state
}
