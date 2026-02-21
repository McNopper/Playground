#include "VulkanRuntime.h"

#include <vector>

#include "core/core.h"

//
// Core Vulkan initialization (no window required)
//

bool VulkanRuntime::prepareInit(bool with_window)
{
	auto result = m_vulkan_setup.init();
	if (!result)
	{
		terminate();

		return false;
	}

	VulkanInstanceFactory vulkan_instance_factory{};
	vulkan_instance_factory.addEnabledLayerName("VK_LAYER_KHRONOS_validation");

	if (with_window)
	{
		uint32_t extension_count{ 0u };
		const char** extensions = glfwGetRequiredInstanceExtensions(&extension_count);
		for (uint32_t i = 0u; i < extension_count; i++)
		{
			vulkan_instance_factory.addEnabledExtensionName(extensions[i]);
		}
		vulkan_instance_factory.addEnabledExtensionName(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME);
	}

	m_instance = vulkan_instance_factory.create();
	if (m_instance == VK_NULL_HANDLE)
	{
		terminate();

		return false;
	}

	return true;
}

bool VulkanRuntime::init()
{
	auto physical_devices = gatherPhysicalDevices(m_instance);

	// Only filter by swapchain extension if surface is available
	if (m_surface != VK_NULL_HANDLE)
	{
		physical_devices = PhysicalDeviceExtensionNameFilter{ VK_KHR_SWAPCHAIN_EXTENSION_NAME } << (PhysicalDeviceTypePriorityFilter{ VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU, VK_PHYSICAL_DEVICE_TYPE_CPU } << physical_devices);
	}
	else
	{
		physical_devices = PhysicalDeviceTypePriorityFilter{ VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU, VK_PHYSICAL_DEVICE_TYPE_CPU } << physical_devices;
	}

	if (physical_devices.empty())
	{
		terminate();

		return false;
	}

	m_physical_device = physical_devices.front();

	auto queue_family_properties = gatherPhysicalDeviceQueueFamilyProperties2(m_physical_device);

	std::vector<uint32_t> queue_family_indices = generateIndices((uint32_t)queue_family_properties.size());

	// Filter by queue flags and optionally by surface support
	if (m_surface != VK_NULL_HANDLE)
	{
		queue_family_indices = QueueFamilyIndexSurfaceFilter{ m_physical_device, m_surface } << (QueueFamilyIndexFlagsFilter{ m_queue_flags, queue_family_properties } << queue_family_indices);
	}
	else
	{
		queue_family_indices = QueueFamilyIndexFlagsFilter{ m_queue_flags, queue_family_properties } << queue_family_indices;
	}

	if (queue_family_indices.empty())
	{
		terminate();

		return false;
	}
	m_queue_family_index = queue_family_indices.front();

	VulkanDeviceFactory vulkan_device_factory{ m_physical_device, m_queue_family_index };

	// Only add presentation extensions if surface is available
	if (m_surface != VK_NULL_HANDLE)
	{
		vulkan_device_factory.addEnabledExtensionName(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		vulkan_device_factory.addEnabledExtensionName(VK_KHR_PRESENT_MODE_FIFO_LATEST_READY_EXTENSION_NAME);
		vulkan_device_factory.addEnabledExtensionName(VK_KHR_PRESENT_ID_2_EXTENSION_NAME);
		vulkan_device_factory.addEnabledExtensionName(VK_KHR_PRESENT_WAIT_2_EXTENSION_NAME);
	}

	m_device = vulkan_device_factory.create();
	if (m_device == VK_NULL_HANDLE)
	{
		terminate();

		return false;
	}

	// If we have a surface, initialize the swapchain automatically
	if (m_surface != VK_NULL_HANDLE && m_window != nullptr)
	{
		m_vulkan_window = std::make_unique<VulkanWindow>(m_physical_device, m_device, m_queue_family_index, m_surface);
		m_vulkan_window->setSurfaceFormat(m_surface_format);
		m_vulkan_window->setSamples(m_samples);
		m_vulkan_window->setDepthStencilFormat(m_depth_stencil_format);
		if (!m_vulkan_window->init())
		{
			m_vulkan_window.reset();

			terminate();

			return false;
		}
	}
	else
	{
		// For non-window case, create command pool and command buffer
		VulkanCommandPoolFactory command_pool_factory{ m_device, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, m_queue_family_index };
		m_command_pool = command_pool_factory.create();
		if (m_command_pool == VK_NULL_HANDLE)
		{
			terminate();

			return false;
		}

		VulkanCommandBufferFactory command_buffer_factory{ m_device, m_command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1u };
		auto command_buffers = command_buffer_factory.create();
		if (command_buffers.empty())
		{
			terminate();

			return false;
		}
		m_command_buffer = command_buffers.front();
	}

	return true;
}

VkPhysicalDevice VulkanRuntime::getPhysicalDevice() const
{
	return m_physical_device;
}

VkDevice VulkanRuntime::getDevice() const
{
	return m_device;
}

uint32_t VulkanRuntime::getQueueFamilyIndex() const
{
	return m_queue_family_index;
}

void VulkanRuntime::setQueueFlags(VkQueueFlags queue_flags)
{
	m_queue_flags = queue_flags;
}

VkQueueFlags VulkanRuntime::getQueueFlags() const
{
	return m_queue_flags;
}

//
// Non-window methods (compute/headless)
//

bool VulkanRuntime::loop(IApplication& application, uint32_t num_loops)
{
	if (m_command_buffer == VK_NULL_HANDLE)
	{
		return false;
	}

	DeltaTime delta_time{};

	for (uint32_t i = 0u; i < num_loops; i++)
	{
		auto current_delta_time = delta_time.tick();

		if (!application.update(current_delta_time, m_command_buffer))
		{
			return false;
		}
	}

	return true;
}

//
// Window-related methods
//

bool VulkanRuntime::prepareInitWindow(int width, int height, const char* title)
{
	if (!glfwInit())
	{
		terminate();

		return false;
	}

	m_glfw_initialized = true;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!m_window)
	{
		terminate();

		return false;
	}

	return true;
}

bool VulkanRuntime::initWindow()
{
	if (m_instance == VK_NULL_HANDLE)
	{
		terminate();

		return false;
	}

	if (m_window == nullptr)
	{
		terminate();

		return false;
	}

	if (m_surface != VK_NULL_HANDLE)
	{
		terminate();

		return false;
	}

	auto vulkan_result = glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface);
	if (vulkan_result != VK_SUCCESS)
	{
		terminate();

		return false;
	}

	return true;
}

bool VulkanRuntime::loopWindow(IApplication& application)
{
	if (!m_vulkan_window)
	{
		return false;
	}

	DeltaTime delta_time{};
	while (!glfwWindowShouldClose(m_window))
	{
		bool should_terminate{ false };

		auto current_delta_time = delta_time.tick();

		if (!glfwGetWindowAttrib(m_window, GLFW_ICONIFIED))
		{
			bool resize{ false };

			VkCommandBuffer command_buffer = m_vulkan_window->beginFrame();
			if (command_buffer != VK_NULL_HANDLE)
			{
				if (!application.update(current_delta_time, command_buffer))
				{
					should_terminate = true;
				}

				if (!m_vulkan_window->endFrame())
				{
					resize = true;
				}
			}
			else
			{
				resize = true;
			}

			if (resize)
			{
				m_vulkan_window->terminate();
				if (!m_vulkan_window->init())
				{
					return false;
				}
			}
		}

		glfwPollEvents();
		if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS || should_terminate)
		{
			glfwSetWindowShouldClose(m_window, 1);
		}
	}

	return true;
}

void VulkanRuntime::terminateWindow()
{
	if (m_vulkan_window)
	{
		m_vulkan_window->terminate();
		m_vulkan_window.reset();
	}

	if (m_surface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
		m_surface = VK_NULL_HANDLE;
	}

	if (m_window)
	{
		glfwDestroyWindow(m_window);
		m_window = nullptr;
	}

	if (m_glfw_initialized)
	{
		glfwTerminate();
		m_glfw_initialized = false;
	}
}

void VulkanRuntime::terminate()
{
	terminateWindow();

	if (m_command_buffer != VK_NULL_HANDLE && m_command_pool != VK_NULL_HANDLE)
	{
		vkFreeCommandBuffers(m_device, m_command_pool, 1u, &m_command_buffer);
		m_command_buffer = VK_NULL_HANDLE;
	}

	if (m_command_pool != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(m_device, m_command_pool, nullptr);
		m_command_pool = VK_NULL_HANDLE;
	}

	if (m_device != VK_NULL_HANDLE)
	{
		vkDestroyDevice(m_device, nullptr);
		m_device = VK_NULL_HANDLE;
	}

	if (m_instance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(m_instance, nullptr);
		m_instance = VK_NULL_HANDLE;
	}

	m_vulkan_setup.terminate();
}

void VulkanRuntime::setSamples(VkSampleCountFlagBits samples)
{
	m_samples = samples;
}

VkSampleCountFlagBits VulkanRuntime::getSamples() const
{
	return m_samples;
}

void VulkanRuntime::setSurfaceFormat(const VkSurfaceFormatKHR& surface_format)
{
	m_surface_format = surface_format;
}

VkSurfaceFormatKHR VulkanRuntime::getSurfaceFormat() const
{
	return m_surface_format;
}

void VulkanRuntime::setDepthStencilFormat(VkFormat depth_stencil_format)
{
	m_depth_stencil_format = depth_stencil_format;
}

VkFormat VulkanRuntime::getDepthStencilFormat() const
{
	return m_depth_stencil_format;
}

IVulkanWindow* VulkanRuntime::getVulkanWindow() const
{
	return m_vulkan_window.get();
}
