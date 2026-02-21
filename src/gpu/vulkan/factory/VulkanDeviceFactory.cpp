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
	addEnabledExtensionName(VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME);
	addEnabledExtensionName(VK_KHR_COOPERATIVE_MATRIX_EXTENSION_NAME);
	addEnabledExtensionName(VK_KHR_COMPUTE_SHADER_DERIVATIVES_EXTENSION_NAME);
	addEnabledExtensionName(VK_KHR_SHADER_CLOCK_EXTENSION_NAME);
	addEnabledExtensionName(VK_KHR_ROBUSTNESS_2_EXTENSION_NAME);

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

	VkPhysicalDeviceFeatures2 physical_device_features2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
	physical_device_features2.features.robustBufferAccess = VK_TRUE;
	physical_device_features2.features.multiDrawIndirect = VK_TRUE;
	physical_device_features2.features.shaderInt16 = VK_TRUE;

	VkPhysicalDeviceVulkan11Features physical_device_vulkan11_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES };
	physical_device_vulkan11_features.storageBuffer16BitAccess = VK_TRUE;
	physical_device_vulkan11_features.shaderDrawParameters = VK_TRUE;

	VkPhysicalDeviceVulkan12Features physical_device_vulkan12_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
	physical_device_vulkan12_features.bufferDeviceAddress = VK_TRUE;
	physical_device_vulkan12_features.timelineSemaphore = VK_TRUE;
	physical_device_vulkan12_features.descriptorIndexing = VK_TRUE;
	physical_device_vulkan12_features.shaderFloat16 = VK_TRUE;
	physical_device_vulkan12_features.shaderInt8 = VK_TRUE;
	physical_device_vulkan12_features.storageBuffer8BitAccess = VK_TRUE;

	VkPhysicalDeviceVulkan13Features physical_device_vulkan13_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
	physical_device_vulkan13_features.maintenance4 = VK_TRUE;
	physical_device_vulkan13_features.synchronization2 = VK_TRUE;
	physical_device_vulkan13_features.dynamicRendering = VK_TRUE;

	VkPhysicalDeviceVulkan14Features physical_device_vulkan14_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES };
	physical_device_vulkan14_features.maintenance5 = VK_TRUE;
	physical_device_vulkan14_features.maintenance6 = VK_TRUE;
	physical_device_vulkan14_features.hostImageCopy = VK_TRUE;

	VkPhysicalDeviceDescriptorBufferFeaturesEXT physical_device_descriptor_buffer_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_FEATURES_EXT };
	physical_device_descriptor_buffer_features.descriptorBuffer = VK_TRUE;

	VkPhysicalDeviceFragmentShadingRateFeaturesKHR physical_device_fragment_shading_rate_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR };
	physical_device_fragment_shading_rate_features.pipelineFragmentShadingRate = VK_TRUE;

	VkPhysicalDeviceCooperativeMatrixFeaturesKHR physical_device_cooperative_matrix_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_KHR };
	physical_device_cooperative_matrix_features.cooperativeMatrix = VK_TRUE;

	VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR physical_device_compute_shader_derivatives_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_SHADER_DERIVATIVES_FEATURES_KHR };
	physical_device_compute_shader_derivatives_features.computeDerivativeGroupQuads = VK_TRUE;
	physical_device_compute_shader_derivatives_features.computeDerivativeGroupLinear = VK_TRUE;

	VkPhysicalDeviceShaderClockFeaturesKHR physical_device_shader_clock_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR };
	physical_device_shader_clock_features.shaderSubgroupClock = VK_TRUE;
	physical_device_shader_clock_features.shaderDeviceClock = VK_TRUE;

	VkPhysicalDeviceRobustness2FeaturesKHR physical_device_robustness2_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_KHR };
	physical_device_robustness2_features.robustBufferAccess2 = VK_TRUE;
	physical_device_robustness2_features.robustImageAccess2 = VK_TRUE;
	physical_device_robustness2_features.nullDescriptor = VK_TRUE;

	VkPhysicalDevicePresentId2FeaturesKHR physical_device_present_id2_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_2_FEATURES_KHR };
	physical_device_present_id2_features.presentId2 = VK_TRUE;

	VkPhysicalDevicePresentWait2FeaturesKHR physical_device_present_wait2_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_2_FEATURES_KHR };
	physical_device_present_wait2_features.presentWait2 = VK_TRUE;

	VkPhysicalDeviceAccelerationStructureFeaturesKHR physical_device_acceleration_structure_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR };
	physical_device_acceleration_structure_features.accelerationStructure = VK_TRUE;

	VkPhysicalDeviceRayTracingPipelineFeaturesKHR physical_device_ray_tracing_pipeline_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR };
	physical_device_ray_tracing_pipeline_features.rayTracingPipeline = VK_TRUE;

	physical_device_features2.pNext = &physical_device_vulkan11_features;
	physical_device_vulkan11_features.pNext = &physical_device_vulkan12_features;
	physical_device_vulkan12_features.pNext = &physical_device_vulkan13_features;
	physical_device_vulkan13_features.pNext = &physical_device_vulkan14_features;
	physical_device_vulkan14_features.pNext = &physical_device_descriptor_buffer_features;
	physical_device_descriptor_buffer_features.pNext = &physical_device_fragment_shading_rate_features;
	physical_device_fragment_shading_rate_features.pNext = &physical_device_cooperative_matrix_features;
	physical_device_cooperative_matrix_features.pNext = &physical_device_compute_shader_derivatives_features;
	physical_device_compute_shader_derivatives_features.pNext = &physical_device_shader_clock_features;
	physical_device_shader_clock_features.pNext = &physical_device_robustness2_features;
	physical_device_robustness2_features.pNext = &physical_device_present_id2_features;
	physical_device_present_id2_features.pNext = &physical_device_present_wait2_features;
	physical_device_present_wait2_features.pNext = &physical_device_acceleration_structure_features;
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
