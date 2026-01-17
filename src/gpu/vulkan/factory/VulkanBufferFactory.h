#ifndef GPU_VULKAN_FACTORY_VULKANBUFFERFACTORY_H_
#define GPU_VULKAN_FACTORY_VULKANBUFFERFACTORY_H_

#include <cstdint>
#include <vector>

#include <volk.h>

class VulkanBufferFactory
{

private:

    VkDevice m_device{ VK_NULL_HANDLE };

    VkDeviceSize m_size{ 0u };
    VkBufferCreateFlags m_flags{ 0u };
    VkBufferUsageFlags  m_usage{ 0u };
    VkSharingMode m_sharing_mode{ VK_SHARING_MODE_EXCLUSIVE };
    std::vector<uint32_t> m_queue_family_indices{};

public:

    VulkanBufferFactory() = delete;

    VulkanBufferFactory(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage);

    VkBuffer create() const;

};

#endif /* GPU_VULKAN_FACTORY_VULKANBUFFERFACTORY_H_ */
