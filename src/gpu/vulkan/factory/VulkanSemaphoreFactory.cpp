#include "VulkanSemaphoreFactory.h"

VulkanSemaphoreFactory::VulkanSemaphoreFactory(VkDevice device, VkSemaphoreCreateFlags semaphore_create_flags) :
	m_device{ device }, m_semaphore_create_flags{ semaphore_create_flags }
{
}

VkSemaphore VulkanSemaphoreFactory::create() const
{
	VkSemaphoreCreateInfo semaphore_create_info{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	semaphore_create_info.flags = m_semaphore_create_flags;

	VkSemaphore semaphore{ VK_NULL_HANDLE };
	vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &semaphore);

	return semaphore;
}
