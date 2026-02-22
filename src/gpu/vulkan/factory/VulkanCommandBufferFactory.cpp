#include "VulkanCommandBufferFactory.h"

VulkanCommandBufferFactory::VulkanCommandBufferFactory(VkDevice device, VkCommandPool command_pool, VkCommandBufferLevel command_buffer_level, uint32_t command_buffer_count) :
    m_device{ device },
    m_command_pool{ command_pool },
    m_command_buffer_level{ command_buffer_level },
    m_command_buffer_count{ command_buffer_count }
{
}

std::vector<VkCommandBuffer> VulkanCommandBufferFactory::create() const
{
    std::vector<VkCommandBuffer> command_buffers{};
    if (m_command_buffer_count == 0u)
    {
        return command_buffers;
    }
    command_buffers.resize(m_command_buffer_count);

    VkCommandBufferAllocateInfo command_buffer_allocate_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    command_buffer_allocate_info.commandPool = m_command_pool;
    command_buffer_allocate_info.level = m_command_buffer_level;
    command_buffer_allocate_info.commandBufferCount = m_command_buffer_count;

    auto result = vkAllocateCommandBuffers(m_device, &command_buffer_allocate_info, command_buffers.data());
    if (result != VK_SUCCESS)
    {
        command_buffers.clear();
    }

    return command_buffers;
}
