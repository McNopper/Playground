#ifndef GPU_VULKAN_UTILITY_HOSTMEMORY_H_
#define GPU_VULKAN_UTILITY_HOSTMEMORY_H_

#include <cstring>
#include <vector>

#include <volk.h>

template<class T>
bool deviceToHost(VkDevice device, VkDeviceMemory device_memory, VkDeviceSize offset, VkDeviceSize size, std::vector<T>& content)
{
    if (size % sizeof(T) != 0u)
    {
        return false;
    }

    void* mapped_memory{ nullptr };
    VkMemoryMapInfo map_info{};
    map_info.sType = VK_STRUCTURE_TYPE_MEMORY_MAP_INFO;
    map_info.memory = device_memory;
    map_info.offset = offset;
    map_info.size = size;

    auto result = vkMapMemory2(device, &map_info, &mapped_memory);
    if (result != VK_SUCCESS)
    {
        return false;
    }

    content.resize(size / sizeof(T));

    std::memcpy(content.data(), mapped_memory, size);

    VkMemoryUnmapInfo unmap_info{};
    unmap_info.sType = VK_STRUCTURE_TYPE_MEMORY_UNMAP_INFO;
    unmap_info.memory = device_memory;

    vkUnmapMemory2(device, &unmap_info);

    return true;
}

template<class T>
bool deviceToHost(VkDevice device, VkDeviceMemory device_memory, VkDeviceSize offset, VkDeviceSize size, T& content)
{
    if (size != sizeof(T))
    {
        return false;
    }

    void* mapped_memory{ nullptr };
    VkMemoryMapInfo map_info{};
    map_info.sType = VK_STRUCTURE_TYPE_MEMORY_MAP_INFO;
    map_info.memory = device_memory;
    map_info.offset = offset;
    map_info.size = size;

    auto result = vkMapMemory2(device, &map_info, &mapped_memory);
    if (result != VK_SUCCESS)
    {
        return false;
    }

    std::memcpy(&content, mapped_memory, size);

    VkMemoryUnmapInfo unmap_info{};
    unmap_info.sType = VK_STRUCTURE_TYPE_MEMORY_UNMAP_INFO;
    unmap_info.memory = device_memory;

    vkUnmapMemory2(device, &unmap_info);

    return true;
}

bool deviceToHost(VkDevice device, VkDeviceMemory device_memory, VkDeviceSize offset, VkDeviceSize size, void* data);

template<class T>
bool hostToDevice(VkDevice device, VkDeviceMemory device_memory, VkDeviceSize offset, VkDeviceSize size, const std::vector<T>& content)
{
    if (size < content.size() * sizeof(T))
    {
        return false;
    }

    void* mapped_memory{ nullptr };
    VkMemoryMapInfo map_info{};
    map_info.sType = VK_STRUCTURE_TYPE_MEMORY_MAP_INFO;
    map_info.memory = device_memory;
    map_info.offset = offset;
    map_info.size = size;

    auto result = vkMapMemory2(device, &map_info, &mapped_memory);
    if (result != VK_SUCCESS)
    {
        return false;
    }

    std::memcpy(mapped_memory, content.data(), content.size() * sizeof(T));

    VkMemoryUnmapInfo unmap_info{};
    unmap_info.sType = VK_STRUCTURE_TYPE_MEMORY_UNMAP_INFO;
    unmap_info.memory = device_memory;

    vkUnmapMemory2(device, &unmap_info);

    return true;
}

template<class T>
bool hostToDevice(VkDevice device, VkDeviceMemory device_memory, VkDeviceSize offset, VkDeviceSize size, const T& content)
{
    if (size < sizeof(T))
    {
        return false;
    }

    void* mapped_memory{ nullptr };
    VkMemoryMapInfo map_info{};
    map_info.sType = VK_STRUCTURE_TYPE_MEMORY_MAP_INFO;
    map_info.memory = device_memory;
    map_info.offset = offset;
    map_info.size = size;

    auto result = vkMapMemory2(device, &map_info, &mapped_memory);
    if (result != VK_SUCCESS)
    {
        return false;
    }

    std::memcpy(mapped_memory, &content, sizeof(T));

    VkMemoryUnmapInfo unmap_info{};
    unmap_info.sType = VK_STRUCTURE_TYPE_MEMORY_UNMAP_INFO;
    unmap_info.memory = device_memory;

    vkUnmapMemory2(device, &unmap_info);

    return true;
}

bool hostToDevice(VkDevice device, VkDeviceMemory device_memory, VkDeviceSize offset, VkDeviceSize size, const void* data);

#endif /* GPU_VULKAN_UTILITY_HOSTMEMORY_H_ */
