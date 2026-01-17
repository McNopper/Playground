#include "VulkanInstanceFactory.h"

#include <cstring>

const std::vector<const char*>& VulkanInstanceFactory::getEnabledLayerNames() const
{
	return m_enabled_layer_names;
}

bool VulkanInstanceFactory::addEnabledLayerName(const char* name)
{
	for (const auto* current_name : m_enabled_layer_names)
	{
		if (strcmp(current_name, name) == 0)
		{
			return false;
		}
	}

	m_enabled_layer_names.push_back(name);

	return true;
}

const std::vector<const char*>& VulkanInstanceFactory::getEnabledExtensionNames() const
{
	return m_enabled_extension_names;
}

bool VulkanInstanceFactory::addEnabledExtensionName(const char* name)
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

VkInstance VulkanInstanceFactory::create() const
{
	VkApplicationInfo application_info{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
	application_info.pApplicationName = m_application_name.c_str();
	application_info.applicationVersion = m_application_version;
	application_info.pEngineName = m_engine_name.c_str();
	application_info.engineVersion = m_engine_version;
	application_info.apiVersion = m_api_version;

	VkInstanceCreateInfo instance_create_info{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	instance_create_info.flags = m_instance_create_flags;
	instance_create_info.pApplicationInfo = &application_info;
	if (!m_enabled_layer_names.empty())
	{
		instance_create_info.enabledLayerCount = (uint32_t)m_enabled_layer_names.size();
		instance_create_info.ppEnabledLayerNames = m_enabled_layer_names.data();
	}
	if (!m_enabled_extension_names.empty())
	{
		instance_create_info.enabledExtensionCount = (uint32_t)m_enabled_extension_names.size();
		instance_create_info.ppEnabledExtensionNames = m_enabled_extension_names.data();
	}

	VkInstance instance{ VK_NULL_HANDLE };
	VkResult result = vkCreateInstance(&instance_create_info, nullptr, &instance);
	if (result != VK_SUCCESS)
	{
		return instance;
	}

	volkLoadInstance(instance);

	return instance;
}
