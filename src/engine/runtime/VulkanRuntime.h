#ifndef ENGINE_RUNTIME_VULKANRUNTIME_H_
#define ENGINE_RUNTIME_VULKANRUNTIME_H_

#include <cstdint>
#include <memory>

#include <volk.h>
#include <GLFW/glfw3.h>

#include "gpu/gpu.h"

#include "IApplication.h"

struct GLFWwindow;

/**
 * VulkanRuntime - Unified Vulkan initialization and window management
 * 
 * Compute-only or graphics-only (headless):
 *   1. prepareInit(false)               // Creates Vulkan instance without window extensions
 *   2. [Optional: setQueueFlags()]      // Set queue type (default: VK_QUEUE_GRAPHICS_BIT)
 *   3. init()                           // Creates device, queue, and command buffer
 *   4. [Initialize application with getPhysicalDevice(), getDevice(), getQueueFamilyIndex()]
 *   5. loop(application, num_loops)     // Execute application for specified iterations
 *   6. [Terminate application]
 *   7. terminate()                      // Cleanup
 * 
 * Graphics with window:
 *   1. prepareInitWindow(width, height, title) // Creates GLFW window
 *   2. prepareInit()                    // Creates Vulkan instance with window extensions
 *   3. initWindow()                     // Creates window surface
 *   4. [Optional: setSamples(), setDepthStencilFormat()] // Configure render settings
 *   5. init()                           // Creates device and swapchain
 *   6. [Initialize application with getPhysicalDevice(), getDevice(), getQueueFamilyIndex(), getVulkanWindow()]
 *   7. loopWindow(application)          // Runs main loop
 *   8. [Terminate application]
 *   9. terminate()                      // Cleanup Vulkan resources
 */
class VulkanRuntime
{

private:

	// Core Vulkan
	VulkanSetup m_vulkan_setup{};

	VkInstance m_instance{ VK_NULL_HANDLE };
	VkPhysicalDevice m_physical_device{ VK_NULL_HANDLE };
	VkDevice m_device{ VK_NULL_HANDLE };
	uint32_t m_queue_family_index{ 0u };
	VkQueueFlags m_queue_flags{ VK_QUEUE_GRAPHICS_BIT };

	// Command buffer for non-window case
	VkCommandPool m_command_pool{ VK_NULL_HANDLE };
	VkCommandBuffer m_command_buffer{ VK_NULL_HANDLE };

	// Window-related
	bool m_glfw_initialized{ false };
	GLFWwindow* m_window{ nullptr };
	VkSurfaceKHR m_surface{ VK_NULL_HANDLE };
	std::unique_ptr<VulkanWindow> m_vulkan_window{};
	VkSampleCountFlagBits m_samples{ VK_SAMPLE_COUNT_1_BIT };
	VkFormat m_depth_stencil_format{ VK_FORMAT_D24_UNORM_S8_UINT };
	VkSurfaceFormatKHR m_surface_format{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

public:

	VulkanRuntime() = default;
	~VulkanRuntime() = default;

	VulkanRuntime(const VulkanRuntime&) = delete;
	VulkanRuntime(VulkanRuntime&&) = delete;

	VulkanRuntime& operator=(const VulkanRuntime&) = delete;
	VulkanRuntime& operator=(VulkanRuntime&&) = delete;

	// Core Vulkan initialization (no window required)
	bool prepareInit(bool with_window = true);

	bool init();

	void terminate();

	VkPhysicalDevice getPhysicalDevice() const;
	VkDevice getDevice() const;
	uint32_t getQueueFamilyIndex() const;

	void setQueueFlags(VkQueueFlags queue_flags);
	VkQueueFlags getQueueFlags() const;

	// Non-window methods (compute/headless)
	bool loop(IApplication& application, uint32_t num_loops = 1u);

	// Window-related methods
	bool prepareInitWindow(int width, int height, const char* title);

	bool initWindow();

	bool loopWindow(IApplication& application);

	void terminateWindow();

	void setSamples(VkSampleCountFlagBits samples);
	VkSampleCountFlagBits getSamples() const;

	void setSurfaceFormat(const VkSurfaceFormatKHR& surface_format);
	VkSurfaceFormatKHR getSurfaceFormat() const;

	void setDepthStencilFormat(VkFormat depth_stencil_format);
	VkFormat getDepthStencilFormat() const;

	IVulkanWindow* getVulkanWindow() const;

};

#endif /* ENGINE_RUNTIME_VULKANRUNTIME_H_ */
