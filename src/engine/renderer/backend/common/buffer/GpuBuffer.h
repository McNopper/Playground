#ifndef RENDERER_BACKEND_COMMON_BUFFER_GPUBUFFER_H_
#define RENDERER_BACKEND_COMMON_BUFFER_GPUBUFFER_H_

#include <vector>

#include <volk.h>

#include "gpu/vulkan/builder/vulkan_resource.h"

class GpuBuffer
{

protected:

    VkPhysicalDevice m_physical_device{ VK_NULL_HANDLE };
    VkDevice m_device{ VK_NULL_HANDLE };

    VulkanBufferResource m_buffer_resource{};
    VkDeviceSize m_device_size{ 0u };
    bool m_host_visible{ true };
    bool m_readback_enabled{ false };

    bool createBuffer(VkDeviceSize size, VkBufferUsageFlags usage);

public:

    GpuBuffer() = delete;
    GpuBuffer(const GpuBuffer& other) = delete;

    GpuBuffer(VkPhysicalDevice physical_device, VkDevice device, bool host_visible = true, bool enable_readback = false);

    virtual ~GpuBuffer();

    GpuBuffer operator=(const GpuBuffer& other) = delete;

    bool create(VkDeviceSize size, VkBufferUsageFlags usage);

    template<typename T>
    bool create(const T& data, VkBufferUsageFlags usage);

    template<typename T>
    bool create(const std::vector<T>& data, VkBufferUsageFlags usage);

    template<typename T>
    bool update(VkDeviceSize offset, const T& data);

    template<typename T>
    bool update(VkDeviceSize offset, const std::vector<T>& data);

    template<typename T>
    bool readBack(VkDeviceSize offset, T& data) const;

    template<typename T>
    bool readBack(VkDeviceSize offset, std::vector<T>& data, size_t count) const;

    VkBuffer getBuffer() const;
    VkDeviceSize getDeviceSize() const;

    bool isValid() const;
    bool isHostVisible() const;
    bool isReadbackEnabled() const;

    void destroy();
};

template<typename T>
bool GpuBuffer::create(const T& data, VkBufferUsageFlags usage)
{
    if (!createBuffer(sizeof(T), usage))
    {
        return false;
    }

    // Only direct write if HOST_VISIBLE
    if (m_host_visible)
    {
        if (!hostToDevice<T>(m_device, m_buffer_resource.device_memory, 0u, sizeof(T), data))
        {
            return false;
        }
    }

    return true;
}

template<typename T>
bool GpuBuffer::create(const std::vector<T>& data, VkBufferUsageFlags usage)
{
    if (data.empty())
    {
        return false;
    }

    VkDeviceSize size = data.size() * sizeof(T);
    if (!createBuffer(size, usage))
    {
        return false;
    }

    // Only direct write if HOST_VISIBLE
    if (m_host_visible)
    {
        if (!hostToDevice<T>(m_device, m_buffer_resource.device_memory, 0u, size, data))
        {
            return false;
        }
    }

    return true;
}

template<typename T>
bool GpuBuffer::update(VkDeviceSize offset, const T& data)
{
    if (!isValid())
    {
        return false;
    }

    // Update only works for HOST_VISIBLE buffers
    if (!isHostVisible())
    {
        return false;
    }

    if (offset + sizeof(T) > getDeviceSize())
    {
        return false;
    }

    if (!hostToDevice<T>(m_device, m_buffer_resource.device_memory, offset, sizeof(T), data))
    {
        return false;
    }

    return true;
}

template<typename T>
bool GpuBuffer::update(VkDeviceSize offset, const std::vector<T>& data)
{
    if (!isValid())
    {
        return false;
    }

    // Update only works for HOST_VISIBLE buffers
    if (!isHostVisible())
    {
        return false;
    }

    if (data.empty())
    {
        return false;
    }

    VkDeviceSize size = data.size() * sizeof(T);
    if (offset + size > getDeviceSize())
    {
        return false;
    }

    if (!hostToDevice<T>(m_device, m_buffer_resource.device_memory, offset, size, data))
    {
        return false;
    }

    return true;
}

template<typename T>
bool GpuBuffer::readBack(VkDeviceSize offset, T& data) const
{
    if (!isValid())
    {
        return false;
    }

    // ReadBack requires readback to be enabled
    if (!isReadbackEnabled())
    {
        return false;
    }

    // ReadBack only works for HOST_VISIBLE buffers
    if (!isHostVisible())
    {
        return false;
    }

    if (offset + sizeof(T) > getDeviceSize())
    {
        return false;
    }

    if (!deviceToHost<T>(m_device, m_buffer_resource.device_memory, offset, sizeof(T), data))
    {
        return false;
    }

    return true;
}

template<typename T>
bool GpuBuffer::readBack(VkDeviceSize offset, std::vector<T>& data, size_t count) const
{
    if (!isValid())
    {
        return false;
    }

    // ReadBack requires readback to be enabled
    if (!isReadbackEnabled())
    {
        return false;
    }

    // ReadBack only works for HOST_VISIBLE buffers
    if (!isHostVisible())
    {
        return false;
    }

    VkDeviceSize size = count * sizeof(T);
    if (offset + size > getDeviceSize())
    {
        return false;
    }

    if (!deviceToHost<T>(m_device, m_buffer_resource.device_memory, offset, size, data))
    {
        return false;
    }

    return true;
}

#endif /* RENDERER_BACKEND_COMMON_BUFFER_GPUBUFFER_H_ */
