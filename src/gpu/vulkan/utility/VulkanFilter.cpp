#include "VulkanFilter.h"

#include <algorithm>
#include <cstring>

#include "vulkan_query.h"

PhysicalDeviceTypePriorityFilter::PhysicalDeviceTypePriorityFilter(std::initializer_list<VkPhysicalDeviceType> physical_device_types) :
	m_physical_device_types{ physical_device_types }
{
}

void PhysicalDeviceTypePriorityFilter::algorithm()
{
	auto temp_data = std::move(m_data);
	m_data.clear();

	for (VkPhysicalDeviceType physical_device_type : m_physical_device_types)
	{
		for (VkPhysicalDevice physical_device : temp_data)
		{
			auto physical_device_properties2 = gatherPhysicalDeviceProperties2(physical_device);

			if (physical_device_properties2.properties.deviceType == physical_device_type)
			{
				m_data.push_back(physical_device);

				break;
			}
		}
	}
}

//

PhysicalDeviceTypeFilter::PhysicalDeviceTypeFilter(VkPhysicalDeviceType physical_device_type) :
	m_physical_device_type{ physical_device_type }
{
}

void PhysicalDeviceTypeFilter::algorithm()
{
	m_data.erase(std::remove_if(m_data.begin(), m_data.end(),
		[&](VkPhysicalDevice physical_device)
		{
			auto physical_device_properties2 = gatherPhysicalDeviceProperties2(physical_device);

			return physical_device_properties2.properties.deviceType != m_physical_device_type;
		}
	), m_data.end());
}

//

PhysicalDeviceExtensionNameFilter::PhysicalDeviceExtensionNameFilter(std::initializer_list<const char*> extension_names) :
	m_extension_names{ extension_names }
{
}

void PhysicalDeviceExtensionNameFilter::algorithm()
{
	m_data.erase(std::remove_if(m_data.begin(), m_data.end(),
		[&](VkPhysicalDevice physical_device)
		{
			auto extension_properties = gatherPhysicalDeviceExtensionProperties(physical_device);

			for (const auto* current_required_extension_name : m_extension_names)
			{
				bool found{ false };

				for (const auto& current_extension_property : extension_properties)
				{
					if (strcmp(current_required_extension_name, current_extension_property.extensionName) == 0)
					{
						found = true;
						break;
					}
				}

				if (!found)
				{
					return true;
				}
			}

			return false;
		}
	), m_data.end());
}

//

QueueFamilyIndexFlagsFilter::QueueFamilyIndexFlagsFilter(VkQueueFlags queue_flags, const std::vector<VkQueueFamilyProperties2>& queue_family_properties2) :
	m_queue_flags{ queue_flags }, m_queue_family_properties2{ queue_family_properties2 }
{
}

void QueueFamilyIndexFlagsFilter::algorithm()
{
	m_data.erase(std::remove_if(m_data.begin(), m_data.end(),
		[&](uint32_t queue_family_index)
		{
			return (m_queue_family_properties2[queue_family_index].queueFamilyProperties.queueFlags & m_queue_flags) != m_queue_flags;
		}
	), m_data.end());
}

//

QueueFamilyIndexSurfaceFilter::QueueFamilyIndexSurfaceFilter(VkPhysicalDevice physical_device, VkSurfaceKHR surface) :
	m_physical_device{ physical_device }, m_surface{ surface }
{
}

void QueueFamilyIndexSurfaceFilter::algorithm()
{
	m_data.erase(std::remove_if(m_data.begin(), m_data.end(),
		[&](uint32_t queue_family_index)
		{
			return gatherPhysicalDeviceSurfaceSupport(m_physical_device, queue_family_index, m_surface) == VK_FALSE;
		}
	), m_data.end());
}

//

MemoryTypeIndexBitsFilter::MemoryTypeIndexBitsFilter(uint32_t memory_type_bits) :
	m_memory_type_bits{ memory_type_bits }
{
}

void MemoryTypeIndexBitsFilter::algorithm()
{
	m_data.erase(std::remove_if(m_data.begin(), m_data.end(),
		[&](uint32_t memory_type_index)
		{
			return !(m_memory_type_bits & (1u << memory_type_index));
		}
	), m_data.end());
}

MemoryTypeIndexPropertyFlagsFilter::MemoryTypeIndexPropertyFlagsFilter(VkMemoryPropertyFlags memory_property_flags, VkPhysicalDeviceMemoryProperties physical_device_memory_properties) :
	m_memory_property_flags{ memory_property_flags }, m_physical_device_memory_properties{ physical_device_memory_properties }
{
}

void MemoryTypeIndexPropertyFlagsFilter::algorithm()
{
	m_data.erase(std::remove_if(m_data.begin(), m_data.end(),
		[&](uint32_t memory_type_index)
		{
			return (m_physical_device_memory_properties.memoryTypes[memory_type_index].propertyFlags & m_memory_property_flags) != m_memory_property_flags;
		}
	), m_data.end());
}

//

FormatImageFilter::FormatImageFilter(VkPhysicalDevice physical_device, VkImageTiling image_tiling, VkFormatFeatureFlags format_feature_flags) :
	m_physical_device{ physical_device }, m_image_tiling{ image_tiling }, m_format_feature_flags{ format_feature_flags }
{
}

void FormatImageFilter::algorithm()
{
	m_data.erase(std::remove_if(m_data.begin(), m_data.end(),
		[&](VkFormat format)
		{
			auto format_properties2 = gatherPhysicalDeviceFormatProperties2(m_physical_device, format);

			bool erase{ true };
			if (m_image_tiling == VK_IMAGE_TILING_OPTIMAL)
			{
				if ((format_properties2.formatProperties.optimalTilingFeatures & m_format_feature_flags) == m_format_feature_flags)
				{
					erase = false;
				}
			}
			else if (m_image_tiling == VK_IMAGE_TILING_LINEAR)
			{
				if ((format_properties2.formatProperties.linearTilingFeatures & m_format_feature_flags) == m_format_feature_flags)
				{
					erase = false;
				}
			}

			return erase;
		}
	), m_data.end());
}

//

SurfaceFormatFilter::SurfaceFormatFilter(VkSurfaceFormatKHR surface_format) :
	m_surface_format{ surface_format }
{
}

void SurfaceFormatFilter::algorithm()
{
	m_data.erase(std::remove_if(m_data.begin(), m_data.end(),
		[&](const VkSurfaceFormatKHR surface_format)
		{
			return !((m_surface_format.format == surface_format.format) && (m_surface_format.colorSpace == surface_format.colorSpace));
		}
	), m_data.end());
}

PresentModeFilter::PresentModeFilter(VkPresentModeKHR present_mode) :
	m_present_mode{ present_mode }
{
}

void PresentModeFilter::algorithm()
{
	m_data.erase(std::remove_if(m_data.begin(), m_data.end(),
		[&](VkPresentModeKHR present_mode)
		{
			return m_present_mode != present_mode;
		}
	), m_data.end());
}
