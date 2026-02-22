#include "vulkan_host_memory.h"

bool deviceToHost(VkDevice device, VkDeviceMemory device_memory, VkDeviceSize offset, VkDeviceSize size, void* data)
{
    if (!data)
    {
        return false;
    }

    void* mapped_memory{ nullptr };
    VkMemoryMapInfo map_info{ VK_STRUCTURE_TYPE_MEMORY_MAP_INFO };
    map_info.memory = device_memory;
    map_info.offset = offset;
    map_info.size = size;

    auto result = vkMapMemory2(device, &map_info, &mapped_memory);
    if (result != VK_SUCCESS)
    {
        return false;
    }

    std::memcpy(data, mapped_memory, size);

    VkMemoryUnmapInfo unmap_info{ VK_STRUCTURE_TYPE_MEMORY_UNMAP_INFO };
    unmap_info.memory = device_memory;

    vkUnmapMemory2(device, &unmap_info);

    return true;
}
bool hostToDevice(VkDevice device, VkDeviceMemory device_memory, VkDeviceSize offset, VkDeviceSize size, const void* data)
{
    if (!data)
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

    std::memcpy(mapped_memory, data, size);

    VkMemoryUnmapInfo unmap_info{};
    unmap_info.sType = VK_STRUCTURE_TYPE_MEMORY_UNMAP_INFO;
    unmap_info.memory = device_memory;

    vkUnmapMemory2(device, &unmap_info);

    return true;
}
