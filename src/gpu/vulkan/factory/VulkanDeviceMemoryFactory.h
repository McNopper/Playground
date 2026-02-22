#ifndef GPU_VULKAN_FACTORY_VULKANDEVICEMEMORYFACTORY_H_
#define GPU_VULKAN_FACTORY_VULKANDEVICEMEMORYFACTORY_H_

#include <volk.h>

class VulkanDeviceMemoryFactory
{

private:

    VkDevice m_device{ VK_NULL_HANDLE };

    VkDeviceSize m_allocation_size{ 0u };
    uint32_t m_memory_type_index{ 0u };

    VkMemoryAllocateFlags m_memory_allocate_flags{ 0u };

public:

    VulkanDeviceMemoryFactory() = delete;

    VulkanDeviceMemoryFactory(VkDevice device, VkDeviceSize allocation_size, uint32_t memory_type_index);

    void setMemoryAllocateFlags(VkMemoryAllocateFlags flags);

    VkDeviceMemory create() const;

};

#endif /* GPU_VULKAN_FACTORY_VULKANDEVICEMEMORYFACTORY_H_ */
