#include "VulkanDeviceMemoryFactory.h"

VulkanDeviceMemoryFactory::VulkanDeviceMemoryFactory(VkDevice device, VkDeviceSize allocation_size, uint32_t memory_type_index) :
    m_device{ device }, m_allocation_size{ allocation_size }, m_memory_type_index{ memory_type_index }
{
}

void VulkanDeviceMemoryFactory::setMemoryAllocateFlags(VkMemoryAllocateFlags flags)
{
    m_memory_allocate_flags = flags;
}

VkDeviceMemory VulkanDeviceMemoryFactory::create() const
{
    VkMemoryAllocateFlagsInfo memory_allocate_flags_info{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO };
    memory_allocate_flags_info.flags = m_memory_allocate_flags;

    VkMemoryAllocateInfo memory_allocate_info{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    memory_allocate_info.pNext = &memory_allocate_flags_info;
    memory_allocate_info.allocationSize = m_allocation_size;
    memory_allocate_info.memoryTypeIndex = m_memory_type_index;

    VkDeviceMemory device_memory{ VK_NULL_HANDLE };
    vkAllocateMemory(m_device, &memory_allocate_info, nullptr, &device_memory);

    return device_memory;
}
