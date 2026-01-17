#include "VulkanDeviceFactory.h"

#include <cstdint>
#include <cstring>

#include "gpu/vulkan/utility/vulkan_query.h"

VulkanDeviceFactory::VulkanDeviceFactory(VkPhysicalDevice physical_device, uint32_t queue_family_index) :
	m_physical_device{ physical_device }, m_queue_family_index{ queue_family_index }
{
	addEnabledExtensionName(VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME);
	// VK_KHR_map_memory2 promoted to Vulkan 1.4 core - no longer needed as extension
	addEnabledExtensionName(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
	addEnabledExtensionName(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
	addEnabledExtensionName(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
}

const std::vector<const char*>& VulkanDeviceFactory::getEnabledExtensionNames() const
{
	return m_enabled_extension_names;
}

bool VulkanDeviceFactory::addEnabledExtensionName(const char* name)
{
	for (const auto* current_name : m_enabled_extension_names)
	{
		if (strcmp(current_name, name) == 0)
		{
			return false;
		}
	}

	m_enabled_extension_names.push_back(name);

	return true;
}

VkDevice VulkanDeviceFactory::create() const
{
	auto current_physical_device_vulkan13_features = gatherPhysicalDeviceVulkan13Features(m_physical_device);
	if (!current_physical_device_vulkan13_features.synchronization2 || !current_physical_device_vulkan13_features.dynamicRendering)
	{
		return VK_NULL_HANDLE;
	}

	VkPhysicalDeviceVulkan11Features physical_device_vulkan11_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES };

	VkPhysicalDeviceVulkan12Features physical_device_vulkan12_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
	physical_device_vulkan12_features.bufferDeviceAddress = VK_TRUE;

	VkPhysicalDeviceVulkan13Features physical_device_vulkan13_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
	physical_device_vulkan13_features.maintenance4 = VK_TRUE;
	physical_device_vulkan13_features.synchronization2 = VK_TRUE;
	physical_device_vulkan13_features.dynamicRendering = VK_TRUE;

	VkPhysicalDeviceVulkan14Features physical_device_vulkan14_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES };
	physical_device_vulkan14_features.maintenance5 = VK_TRUE;
	physical_device_vulkan14_features.maintenance6 = VK_TRUE;

	VkPhysicalDeviceDescriptorBufferFeaturesEXT physical_device_descriptor_buffer_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_FEATURES_EXT };
	physical_device_descriptor_buffer_features.descriptorBuffer = VK_TRUE;

	VkPhysicalDeviceAccelerationStructureFeaturesKHR physical_device_acceleration_structure_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR };
	physical_device_acceleration_structure_features.accelerationStructure = VK_TRUE;

	VkPhysicalDeviceRayTracingPipelineFeaturesKHR physical_device_ray_tracing_pipeline_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR };
	physical_device_ray_tracing_pipeline_features.rayTracingPipeline = VK_TRUE;

	VkPhysicalDeviceFeatures2 physical_device_features2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
	physical_device_features2.pNext = &physical_device_vulkan11_features;
	physical_device_vulkan11_features.pNext = &physical_device_vulkan12_features;
	physical_device_vulkan12_features.pNext = &physical_device_vulkan13_features;
	physical_device_vulkan13_features.pNext = &physical_device_vulkan14_features;
	physical_device_vulkan14_features.pNext = &physical_device_descriptor_buffer_features;
	physical_device_descriptor_buffer_features.pNext = &physical_device_acceleration_structure_features;
	physical_device_acceleration_structure_features.pNext = &physical_device_ray_tracing_pipeline_features;

	//

	VkDeviceQueueCreateInfo device_queue_create_info{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
	device_queue_create_info.flags = m_device_queue_create_flags;
	device_queue_create_info.queueFamilyIndex = m_queue_family_index;
	device_queue_create_info.queueCount = (uint32_t)m_queue_priorities.size();
	device_queue_create_info.pQueuePriorities = m_queue_priorities.data();

	VkDeviceCreateInfo device_create_info{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	device_create_info.pNext = &physical_device_features2;
	device_create_info.flags = m_device_create_flags;
	device_create_info.queueCreateInfoCount = 1u;
	device_create_info.pQueueCreateInfos = &device_queue_create_info;
	device_create_info.enabledExtensionCount = (uint32_t)m_enabled_extension_names.size();
	device_create_info.ppEnabledExtensionNames = m_enabled_extension_names.data();

	VkDevice device{ VK_NULL_HANDLE };
	VkResult result = vkCreateDevice(m_physical_device, &device_create_info, nullptr, &device);
	if (result != VK_SUCCESS)
	{
		return device;
	}

	volkLoadDevice(device);

	return device;
}
