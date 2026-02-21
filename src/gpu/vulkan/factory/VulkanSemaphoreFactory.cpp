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

VkSemaphore VulkanSemaphoreFactory::createTimeline(uint64_t initial_value) const
{
	VkSemaphoreTypeCreateInfo semaphore_type_create_info{ VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO };
	semaphore_type_create_info.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
	semaphore_type_create_info.initialValue = initial_value;

	VkSemaphoreCreateInfo semaphore_create_info{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	semaphore_create_info.pNext = &semaphore_type_create_info;
	semaphore_create_info.flags = m_semaphore_create_flags;

	VkSemaphore semaphore{ VK_NULL_HANDLE };
	vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &semaphore);

	return semaphore;
}
