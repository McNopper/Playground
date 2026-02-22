#include "VulkanBufferFactory.h"

VulkanBufferFactory::VulkanBufferFactory(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage) :
    m_device{ device }, m_size{ size }, m_usage{ usage }
{
}

VkBuffer VulkanBufferFactory::create() const
{
    VkBufferCreateInfo buffer_create_info{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    buffer_create_info.flags = m_flags;
    buffer_create_info.size = m_size;
    buffer_create_info.usage = m_usage;
    buffer_create_info.sharingMode = m_sharing_mode;
    if (!m_queue_family_indices.empty())
    {
        buffer_create_info.queueFamilyIndexCount = (uint32_t)m_queue_family_indices.size();
        buffer_create_info.pQueueFamilyIndices = m_queue_family_indices.data();
    }

    VkBuffer buffer{ VK_NULL_HANDLE };
    vkCreateBuffer(m_device, &buffer_create_info, nullptr, &buffer);

    return buffer;
}
