#ifndef GPU_VULKAN_PRESENTATION_VULKANWINDOW_H_
#define GPU_VULKAN_PRESENTATION_VULKANWINDOW_H_

#include <vector>

#include <volk.h>

#include "IVulkanWindow.h"
#include "core/math/vector.h"
#include "gpu/vulkan/builder/vulkan_resource.h"

struct VulkanSwapchainImageResource
{
    VkImage image{ VK_NULL_HANDLE };
    VkImageView image_view{ VK_NULL_HANDLE };

    VkSemaphore signal_semaphore{ VK_NULL_HANDLE };
};

struct VulkanFrameResource
{
    VkCommandBuffer command_buffer{ VK_NULL_HANDLE };

    VkFence fence{ VK_NULL_HANDLE };

    VkSemaphore wait_semaphore{ VK_NULL_HANDLE };
};

class VulkanWindow : public IVulkanWindow
{

private:

    VkPhysicalDevice m_physical_device{ VK_NULL_HANDLE };
    VkDevice m_device{ VK_NULL_HANDLE };
    uint32_t m_queue_family_index{ 0u };
    VkQueue m_queue{ VK_NULL_HANDLE };
    VkSurfaceKHR m_surface{ VK_NULL_HANDLE };

    VkSurfaceFormatKHR m_surface_format{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
    VkPresentModeKHR m_present_mode{ VK_PRESENT_MODE_FIFO_KHR };

    std::vector<VkImageMemoryBarrier2> m_render_image_memory_barriers{};
    std::vector<VkImageMemoryBarrier2> m_present_image_memory_barriers{};

    VkExtent2D m_current_extent{};

    VkSampleCountFlagBits m_samples{ VK_SAMPLE_COUNT_1_BIT };
    VulkanImageResource m_msaa_image_resource{};
    int32_t m_msaa_image_barrier_index{ -1 };

    VkFormat m_depth_stencil_format{ VK_FORMAT_UNDEFINED };
    VulkanImageResource m_depth_stencil_image_resource{};
    int32_t m_depth_stencil_image_barrier_index{ -1 };

    VkSwapchainKHR m_swapchain{ VK_NULL_HANDLE };

    VkCommandPool m_command_pool{ VK_NULL_HANDLE };

    std::vector<VulkanSwapchainImageResource> m_swapchain_image_resources{};
    uint32_t m_swapchain_image_index{ 0u };

    std::vector<VulkanFrameResource> m_frame_resources{};
    // Note: Current swapchain index does not need to be the same as the current frame index!
    uint32_t m_frame_index{ 0u };

    float4 m_clear_color{ 0.0f, 0.0f, 0.0f, 1.0f };
    float m_clear_depth{ 1.0f };
    uint32_t m_clear_stencil{ 0u };

    uint64_t m_present_id{ 0u };
    bool m_present_id2_enabled{ false };

public:

    VulkanWindow() = delete;

    VulkanWindow(const VulkanWindow&) = delete;
    VulkanWindow(VulkanWindow&&) = delete;

    VulkanWindow operator=(const VulkanWindow&) = delete;
    VulkanWindow operator=(VulkanWindow&&) = delete;

    VulkanWindow(VkPhysicalDevice physical_device, VkDevice device, uint32_t queue_family_index, VkSurfaceKHR surface);

    void setSurfaceFormat(const VkSurfaceFormatKHR& surface_format);
    void setPresentMode(VkPresentModeKHR present_mode);

    void setSamples(VkSampleCountFlagBits samples);

    void setDepthStencilFormat(VkFormat depth_stencil_format);

    bool init();

    VkCommandBuffer beginFrame();

    bool endFrame();

    void terminate();

    // Exposed to application.

    VkExtent2D getCurrentExtent() const override;

    VkSurfaceFormatKHR getSurfaceFormat() const override;

    VkFormat getDepthStencilFormat() const override;

    VkSampleCountFlagBits getSamples() const override;

    void setClearColor(float red, float green, float blue, float alpha) override;

    void setClearColor(const float4& clear_color) override;

    void setClearDepth(float depth) override;

    void setClearStencil(uint32_t stencil) override;

    void beginRendering() const override;

    void endRendering() const override;

    uint64_t getPresentId() const;

    VkResult waitForPresent(uint64_t present_id, uint64_t timeout = UINT64_MAX) const;
};

#endif /* GPU_VULKAN_PRESENTATION_VULKANWINDOW_H_ */
