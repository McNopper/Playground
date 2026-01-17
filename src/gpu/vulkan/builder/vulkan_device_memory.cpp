#include "vulkan_device_memory.h"

#include "core/utility/generator.h"

#include "gpu/vulkan/utility/VulkanFilter.h"
#include "gpu/vulkan/utility/vulkan_query.h"

#include "gpu/vulkan/factory/VulkanDeviceMemoryFactory.h"

VkDeviceMemory buildImageDeviceMemory(VkPhysicalDevice physical_device, VkDevice device, VkImage image, VkMemoryPropertyFlags memory_property_flags)
{
	VkDeviceMemory device_memory{VK_NULL_HANDLE};

	auto image_memory_requirements = gatherDeviceImageMemoryRequirements2(device, image);

	auto physical_device_memory_properties = gatherPhysicalDeviceMemoryProperties2(physical_device);
	std::vector<uint32_t> memory_type_indices = generateIndices(physical_device_memory_properties.memoryProperties.memoryTypeCount);
	memory_type_indices = MemoryTypeIndexPropertyFlagsFilter{ memory_property_flags, physical_device_memory_properties.memoryProperties } << (MemoryTypeIndexBitsFilter{ image_memory_requirements.memoryRequirements.memoryTypeBits } << memory_type_indices);
	if (memory_type_indices.empty())
	{
		return device_memory;
	}

	VulkanDeviceMemoryFactory device_memory_factory{ device, image_memory_requirements.memoryRequirements.size, memory_type_indices[0] };
	device_memory = device_memory_factory.create();
	if (device_memory == VK_NULL_HANDLE)
	{
		return device_memory;
	}

	auto result = vkBindImageMemory(device, image, device_memory, 0u);
	if (result != VK_SUCCESS)
	{
		vkFreeMemory(device, device_memory, nullptr);
		device_memory = VK_NULL_HANDLE;
	}

	return device_memory;
}

VkDeviceMemory buildBufferDeviceMemory(VkPhysicalDevice physical_device, VkDevice device, VkBuffer buffer, VkMemoryPropertyFlags memory_property_flags, VkMemoryAllocateFlags memory_allocate_flags)
{
	VkDeviceMemory device_memory{ VK_NULL_HANDLE };

	auto buffer_memory_requirements = gatherDeviceBufferMemoryRequirements2(device, buffer);

	auto physical_device_memory_properties = gatherPhysicalDeviceMemoryProperties2(physical_device);
	std::vector<uint32_t> memory_type_indices = generateIndices(physical_device_memory_properties.memoryProperties.memoryTypeCount);
	memory_type_indices = MemoryTypeIndexPropertyFlagsFilter{ memory_property_flags, physical_device_memory_properties.memoryProperties } << (MemoryTypeIndexBitsFilter{ buffer_memory_requirements.memoryRequirements.memoryTypeBits } << memory_type_indices);
	if (memory_type_indices.empty())
	{
		return device_memory;
	}

	VulkanDeviceMemoryFactory device_memory_factory{ device, buffer_memory_requirements.memoryRequirements.size, memory_type_indices[0] };
	device_memory_factory.setMemoryAllocateFlags(memory_allocate_flags);
	device_memory = device_memory_factory.create();
	if (device_memory == VK_NULL_HANDLE)
	{
		return device_memory;
	}

	auto result = vkBindBufferMemory(device, buffer, device_memory, 0u);
	if (result != VK_SUCCESS)
	{
		vkFreeMemory(device, device_memory, nullptr);
		device_memory = VK_NULL_HANDLE;
	}

	return device_memory;
}
