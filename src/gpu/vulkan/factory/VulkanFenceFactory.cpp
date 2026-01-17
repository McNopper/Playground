#include "VulkanFenceFactory.h"

VulkanFenceFactory::VulkanFenceFactory(VkDevice device, VkFenceCreateFlags fence_create_flags) :
	m_device{ device }, m_fence_create_flags{ fence_create_flags }
{
}

VkFence VulkanFenceFactory::create() const
{
	VkFenceCreateInfo fence_create_info{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	fence_create_info.flags = m_fence_create_flags;

	VkFence fence{ VK_NULL_HANDLE };
	vkCreateFence(m_device, &fence_create_info, nullptr, &fence);

	return fence;
}
