#include "VulkanCommandPoolFactory.h"

VulkanCommandPoolFactory::VulkanCommandPoolFactory(VkDevice device, VkCommandPoolCreateFlags command_pool_create_flags, uint32_t queue_family_index) :
    m_device{ device }, m_command_pool_create_flags{ command_pool_create_flags }, m_queue_family_index{ queue_family_index }
{
}

VkCommandPool VulkanCommandPoolFactory::create() const
{
    VkCommandPoolCreateInfo command_pool_create_info{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
    command_pool_create_info.flags = m_command_pool_create_flags;
    command_pool_create_info.queueFamilyIndex = m_queue_family_index;

    VkCommandPool command_pool{ VK_NULL_HANDLE };
    vkCreateCommandPool(m_device, &command_pool_create_info, nullptr, &command_pool);

    return command_pool;
}
