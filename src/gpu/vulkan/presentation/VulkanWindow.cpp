#include "VulkanWindow.h"

#include <algorithm>

#include "gpu/vulkan/builder/vulkan_device_memory.h"
#include "gpu/vulkan/factory/VulkanCommandBufferFactory.h"
#include "gpu/vulkan/factory/VulkanCommandPoolFactory.h"
#include "gpu/vulkan/factory/VulkanDeviceMemoryFactory.h"
#include "gpu/vulkan/factory/VulkanFenceFactory.h"
#include "gpu/vulkan/factory/VulkanImageFactory.h"
#include "gpu/vulkan/factory/VulkanImageViewFactory.h"
#include "gpu/vulkan/factory/VulkanSemaphoreFactory.h"
#include "gpu/vulkan/factory/VulkanSwapchainFactory.h"
#include "gpu/vulkan/utility/VulkanFilter.h"
#include "gpu/vulkan/utility/VulkanFrame.h"
#include "gpu/vulkan/utility/vulkan_helper.h"
#include "gpu/vulkan/utility/vulkan_query.h"

VulkanWindow::VulkanWindow(VkPhysicalDevice physical_device, VkDevice device, uint32_t queue_family_index, VkSurfaceKHR surface) :
    m_physical_device{ physical_device },
    m_device{ device },
    m_queue_family_index{ queue_family_index },
    m_surface{ surface }
{
}

VkExtent2D VulkanWindow::getCurrentExtent() const
{
    return m_current_extent;
}

VkSurfaceFormatKHR VulkanWindow::getSurfaceFormat() const
{
    return m_surface_format;
}

VkFormat VulkanWindow::getDepthStencilFormat() const
{
    return m_depth_stencil_format;
}

VkSampleCountFlagBits VulkanWindow::getSamples() const
{
    return m_samples;
}

void VulkanWindow::setSurfaceFormat(const VkSurfaceFormatKHR& surface_format)
{
    m_surface_format = surface_format;
}

void VulkanWindow::setPresentMode(VkPresentModeKHR present_mode)
{
    m_present_mode = present_mode;
}

void VulkanWindow::setSamples(VkSampleCountFlagBits samples)
{
    m_samples = samples;
}

void VulkanWindow::setDepthStencilFormat(VkFormat depth_stencil_format)
{
    m_depth_stencil_format = depth_stencil_format;
}

bool VulkanWindow::init()
{
    vkGetDeviceQueue(m_device, m_queue_family_index, 0, &m_queue);

    // Barriers to switch between rendering and presenting layout.

    VkImageMemoryBarrier2 image_memory_barrier2{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
    image_memory_barrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier2.image = VK_NULL_HANDLE;
    image_memory_barrier2.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u };

    image_memory_barrier2.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
    image_memory_barrier2.srcAccessMask = 0u;
    image_memory_barrier2.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    image_memory_barrier2.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    image_memory_barrier2.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_memory_barrier2.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    m_render_image_memory_barriers.push_back(image_memory_barrier2);

    image_memory_barrier2.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    image_memory_barrier2.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    image_memory_barrier2.dstStageMask = VK_PIPELINE_STAGE_2_NONE;
    image_memory_barrier2.dstAccessMask = 0u;
    image_memory_barrier2.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    image_memory_barrier2.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    m_present_image_memory_barriers.push_back(image_memory_barrier2);

    // Gathering information about the surface.

    auto surface_capabilities = gatherPhysicalDeviceSurfaceCapabilities(m_physical_device, m_surface);
    if (!surface_capabilities.has_value())
    {
        return false;
    }
    m_current_extent = surface_capabilities->currentExtent;

    auto surface_formats = gatherPhysicalDeviceSurfaceFormats(m_physical_device, m_surface);
    surface_formats = SurfaceFormatFilter{ m_surface_format } << surface_formats;
    if (surface_formats.empty())
    {
        return false;
    }

    auto surface_present_modes = gatherPhysicalDeviceSurfacePresentModes(m_physical_device, m_surface);
    surface_present_modes = PresentModeFilter{ m_present_mode } << surface_present_modes;
    if (surface_present_modes.empty())
    {
        return false;
    }

    auto physical_device_properties2 = gatherPhysicalDeviceProperties2(m_physical_device);

    // Optional multisample buffer creation.

    if (m_samples != VK_SAMPLE_COUNT_1_BIT)
    {
        // Check for valid samples in advance.

        if ((physical_device_properties2.properties.limits.framebufferColorSampleCounts & m_samples) != m_samples)
        {
            return false;
        }

        VulkanImageFactory image_factory{ m_device, m_surface_format.format, { m_current_extent.width, m_current_extent.height, 1u }, m_samples, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
        m_msaa_image_resource.image = image_factory.create();
        if (m_msaa_image_resource.image == VK_NULL_HANDLE)
        {
            return false;
        }

        // Create the image and the required device memory.

        m_msaa_image_resource.device_memory = buildImageDeviceMemory(m_physical_device, m_device, m_msaa_image_resource.image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        if (m_msaa_image_resource.device_memory == VK_NULL_HANDLE)
        {
            return false;
        }

        // Plus the image view.

        VulkanImageViewFactory image_view_factory{ m_device, m_msaa_image_resource.image, VK_IMAGE_VIEW_TYPE_2D, m_surface_format.format, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u } };
        m_msaa_image_resource.image_view = image_view_factory.create();
        if (m_msaa_image_resource.image_view == VK_NULL_HANDLE)
        {
            return false;
        }

        // Add a image barriers as well.

        m_msaa_image_barrier_index = (int32_t)m_render_image_memory_barriers.size();

        image_memory_barrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        image_memory_barrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        image_memory_barrier2.image = m_msaa_image_resource.image;
        image_memory_barrier2.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u };

        image_memory_barrier2.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
        image_memory_barrier2.srcAccessMask = 0u;
        image_memory_barrier2.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        image_memory_barrier2.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        image_memory_barrier2.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_memory_barrier2.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        m_render_image_memory_barriers.push_back(image_memory_barrier2);

        image_memory_barrier2.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        image_memory_barrier2.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
        image_memory_barrier2.dstStageMask = VK_PIPELINE_STAGE_2_NONE;
        image_memory_barrier2.dstAccessMask = 0u;
        image_memory_barrier2.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        image_memory_barrier2.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        m_present_image_memory_barriers.push_back(image_memory_barrier2);
    }

    // Optional depth buffer creation.

    if (m_depth_stencil_format != VK_FORMAT_UNDEFINED)
    {
        std::vector<VkFormat> depth_stencil_formats{
            m_depth_stencil_format
        };

        depth_stencil_formats = FormatImageFilter{ m_physical_device, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT } << depth_stencil_formats;
        if (depth_stencil_formats.empty())
        {
            return false;
        }

        // Check samples for depth.
        if (isFormatDepthStencilOnly(m_depth_stencil_format) || isFormatDepthOnly(m_depth_stencil_format))
        {
            if ((physical_device_properties2.properties.limits.framebufferDepthSampleCounts & m_samples) != m_samples)
            {
                return false;
            }
        }

        // Check samples for stencil.
        if (isFormatDepthStencilOnly(m_depth_stencil_format) || isFormatStencilOnly(m_depth_stencil_format))
        {
            if ((physical_device_properties2.properties.limits.framebufferStencilSampleCounts & m_samples) != m_samples)
            {
                return false;
            }
        }

        VulkanImageFactory image_factory{ m_device, m_depth_stencil_format, { m_current_extent.width, m_current_extent.height, 1u }, m_samples, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT };
        m_depth_stencil_image_resource.image = image_factory.create();
        if (m_depth_stencil_image_resource.image == VK_NULL_HANDLE)
        {
            return false;
        }

        // Create the image and the required device memory.

        m_depth_stencil_image_resource.device_memory = buildImageDeviceMemory(m_physical_device, m_device, m_depth_stencil_image_resource.image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        if (m_depth_stencil_image_resource.device_memory == VK_NULL_HANDLE)
        {
            return false;
        }

        // Plus the image view.

        VulkanImageViewFactory image_view_factory{ m_device, m_depth_stencil_image_resource.image, VK_IMAGE_VIEW_TYPE_2D, m_depth_stencil_format, { VK_IMAGE_ASPECT_DEPTH_BIT, 0u, 1u, 0u, 1u } };
        m_depth_stencil_image_resource.image_view = image_view_factory.create();
        if (m_depth_stencil_image_resource.image_view == VK_NULL_HANDLE)
        {
            return false;
        }

        // Add a image barriers as well.

        m_depth_stencil_image_barrier_index = (int32_t)m_render_image_memory_barriers.size();

        image_memory_barrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        image_memory_barrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        image_memory_barrier2.image = m_depth_stencil_image_resource.image;
        image_memory_barrier2.subresourceRange = { 0u, 0u, 1u, 0u, 1u };
        if (isFormatDepthStencilOnly(m_depth_stencil_format) || isFormatDepthOnly(m_depth_stencil_format))
        {
            image_memory_barrier2.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        if (isFormatDepthStencilOnly(m_depth_stencil_format) || isFormatStencilOnly(m_depth_stencil_format))
        {
            image_memory_barrier2.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }

        image_memory_barrier2.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
        image_memory_barrier2.srcAccessMask = 0u;
        image_memory_barrier2.dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
        image_memory_barrier2.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        image_memory_barrier2.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_memory_barrier2.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        m_render_image_memory_barriers.push_back(image_memory_barrier2);
    }

    // Swapchain creation.

    VkSwapchainCreateFlagsKHR swapchain_flags{ 0u };

    // Check if VkPresentId2KHR is supported by the surface (requires VK_KHR_get_surface_capabilities2 instance ext).
    if (vkGetPhysicalDeviceSurfaceCapabilities2KHR != nullptr)
    {
        VkSurfaceCapabilitiesPresentId2KHR present_id2_caps{ VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_PRESENT_ID_2_KHR };
        VkPhysicalDeviceSurfaceInfo2KHR surface_info2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR };
        surface_info2.surface = m_surface;
        VkSurfaceCapabilities2KHR surface_caps2{ VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR };
        surface_caps2.pNext = &present_id2_caps;
        if (vkGetPhysicalDeviceSurfaceCapabilities2KHR(m_physical_device, &surface_info2, &surface_caps2) == VK_SUCCESS && present_id2_caps.presentId2Supported == VK_TRUE)
        {
            m_present_id2_enabled = true;
            swapchain_flags |= VK_SWAPCHAIN_CREATE_PRESENT_ID_2_BIT_KHR;
        }
    }

    VulkanSwapchainFactory swapchain_factory{ m_physical_device, m_device, m_surface, surface_formats[0], surface_present_modes[0] };
    swapchain_factory.setSwapchainCreateFlags(swapchain_flags);
    m_swapchain = swapchain_factory.create();
    if (m_swapchain == VK_NULL_HANDLE)
    {
        return false;
    }

    auto swapchain_images = gatherDeviceSwapchainImages(m_device, m_swapchain);
    if (swapchain_images.empty())
    {
        return false;
    }

    uint32_t number_frames = (uint32_t)swapchain_images.size();

    // Command pool creation and command buffer preparations.

    VulkanCommandPoolFactory command_pool_factory{ m_device, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, m_queue_family_index };
    m_command_pool = command_pool_factory.create();
    if (m_command_pool == VK_NULL_HANDLE)
    {
        return false;
    }

    VulkanCommandBufferFactory command_buffer_factory{ m_device, m_command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, number_frames };

    auto command_buffers = command_buffer_factory.create();
    if (command_buffers.empty())
    {
        return false;
    }

    // For each frame ...

    VulkanSemaphoreFactory semaphore_factory{ m_device, 0u };
    VulkanFenceFactory fence_factory{ m_device, VK_FENCE_CREATE_SIGNALED_BIT };

    for (uint32_t i = 0u; i < number_frames; i++)
    {
        //
        // Create swapchain image resources.
        //

        VulkanSwapchainImageResource swapchain_image_resource{};

        // Add swapchain image ..

        swapchain_image_resource.image = swapchain_images[i];

        // ... and image view.
        VulkanImageViewFactory image_view_factory{ m_device, swapchain_images[i], VK_IMAGE_VIEW_TYPE_2D, surface_formats[0].format, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u } };
        swapchain_image_resource.image_view = image_view_factory.create();

        // Semaphores

        swapchain_image_resource.signal_semaphore = semaphore_factory.create();

        m_swapchain_image_resources.push_back(swapchain_image_resource);

        //
        // Creating frame resources.
        //

        VulkanFrameResource vulkan_frame_resource{};

        // Assigning a command buffer.

        vulkan_frame_resource.command_buffer = command_buffers[i];

        // A fence.

        vulkan_frame_resource.fence = fence_factory.create();

        // Semaphores.

        vulkan_frame_resource.wait_semaphore = semaphore_factory.create();

        // Store and process next frame resource.

        m_frame_resources.push_back(vulkan_frame_resource);
    }

    return true;
}

void VulkanWindow::setClearColor(float red, float green, float blue, float alpha)
{
    m_clear_color = { red, green, blue, alpha };
}

void VulkanWindow::setClearColor(const float4& clear_color)
{
    m_clear_color = clear_color;
}

void VulkanWindow::setClearDepth(float depth)
{
    m_clear_depth = depth;
}

void VulkanWindow::setClearStencil(uint32_t stencil)
{
    m_clear_stencil = stencil;
}

VkCommandBuffer VulkanWindow::beginFrame()
{
    // Wait for the fence of a given frame, that we do not use a command in the queue.

    auto result = vkWaitForFences(m_device, 1u, &m_frame_resources[m_frame_index].fence, VK_TRUE, UINT64_MAX);
    if (result != VK_SUCCESS)
    {
        return VK_NULL_HANDLE;
    }

    result = vkResetFences(m_device, 1u, &m_frame_resources[m_frame_index].fence);
    if (result != VK_SUCCESS)
    {
        return VK_NULL_HANDLE;
    }

    // Note: The system provides the swapchain image index.
    result = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, m_frame_resources[m_frame_index].wait_semaphore, VK_NULL_HANDLE, &m_swapchain_image_index);
    if (result != VK_SUCCESS)
    {
        return VK_NULL_HANDLE;
    }

    VkCommandBufferBeginInfo command_buffer_begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    result = vkBeginCommandBuffer(m_frame_resources[m_frame_index].command_buffer, &command_buffer_begin_info);
    if (result != VK_SUCCESS)
    {
        return VK_NULL_HANDLE;
    }

    // Change the layouts for rendering.

    m_render_image_memory_barriers[0].image = m_swapchain_image_resources[m_swapchain_image_index].image;
    // Note: For optional MSAA and/or depth, the images do not change.

    VkDependencyInfo dependency_info{ VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
    dependency_info.imageMemoryBarrierCount = (uint32_t)m_render_image_memory_barriers.size();
    dependency_info.pImageMemoryBarriers = m_render_image_memory_barriers.data();
    vkCmdPipelineBarrier2(m_frame_resources[m_frame_index].command_buffer, &dependency_info);

    return m_frame_resources[m_frame_index].command_buffer;
}

void VulkanWindow::beginRendering() const
{
    // Prepare the begin of rendering.

    VulkanFrame vulkan_frame{ m_swapchain_image_resources[m_swapchain_image_index].image_view, { { 0, 0 }, m_current_extent } };
    vulkan_frame.setClearColor(m_clear_color);

    if (m_samples != VK_SAMPLE_COUNT_1_BIT)
    {
        vulkan_frame.setMsaaImageView(m_msaa_image_resource.image_view);
    }

    if (m_depth_stencil_format != VK_FORMAT_UNDEFINED)
    {
        if (isFormatDepthStencilOnly(m_depth_stencil_format) || isFormatDepthOnly(m_depth_stencil_format))
        {
            vulkan_frame.setClearDepth(m_clear_depth);
            vulkan_frame.setDepthImageView(m_depth_stencil_image_resource.image_view);
        }
        if (isFormatDepthStencilOnly(m_depth_stencil_format) || isFormatStencilOnly(m_depth_stencil_format))
        {
            vulkan_frame.setClearStencil(m_clear_stencil);
            vulkan_frame.setStencilImageView(m_depth_stencil_image_resource.image_view);
        }
    }

    // Begin of rendering, which can be decided to be executed during an update loop.

    vulkan_frame.beginRendering(m_frame_resources[m_frame_index].command_buffer);
}

void VulkanWindow::endRendering() const
{
    // End of rendering.

    vkCmdEndRendering(m_frame_resources[m_frame_index].command_buffer);
}

bool VulkanWindow::endFrame()
{
    // Switch back layouts for presentation.

    m_present_image_memory_barriers[0].image = m_swapchain_image_resources[m_swapchain_image_index].image;
    // Note: For optional MSAA and/or depth, the images do not change.

    VkDependencyInfo dependency_info{ VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
    dependency_info.imageMemoryBarrierCount = (uint32_t)m_present_image_memory_barriers.size();
    dependency_info.pImageMemoryBarriers = m_present_image_memory_barriers.data();
    vkCmdPipelineBarrier2(m_frame_resources[m_frame_index].command_buffer, &dependency_info);

    // End of command.

    auto result = vkEndCommandBuffer(m_frame_resources[m_frame_index].command_buffer);
    if (result != VK_SUCCESS)
    {
        return false;
    }

    // Prepare command submit to queue.

    VkSemaphoreSubmitInfo wait_semaphore_info{ VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
    wait_semaphore_info.semaphore = m_frame_resources[m_frame_index].wait_semaphore;
    wait_semaphore_info.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkCommandBufferSubmitInfo command_buffer_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
    command_buffer_info.commandBuffer = m_frame_resources[m_frame_index].command_buffer;

    VkSemaphoreSubmitInfo signal_semaphore_info{ VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
    signal_semaphore_info.semaphore = m_swapchain_image_resources[m_swapchain_image_index].signal_semaphore;
    signal_semaphore_info.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;

    VkSubmitInfo2 submit_info2{ VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
    submit_info2.waitSemaphoreInfoCount = 1u;
    submit_info2.pWaitSemaphoreInfos = &wait_semaphore_info;
    submit_info2.commandBufferInfoCount = 1u;
    submit_info2.pCommandBufferInfos = &command_buffer_info;
    submit_info2.signalSemaphoreInfoCount = 1u;
    submit_info2.pSignalSemaphoreInfos = &signal_semaphore_info;

    // As soon as commands are executed, fence is signaled.
    result = vkQueueSubmit2(m_queue, 1u, &submit_info2, m_frame_resources[m_frame_index].fence);
    if (result != VK_SUCCESS)
    {
        return false;
    }

    // Prepare, that present happens as soon as previous signal semaphore was triggered.
    m_present_id++;

    VkPresentInfoKHR present_info{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    present_info.waitSemaphoreCount = 1u;
    present_info.pWaitSemaphores = &m_swapchain_image_resources[m_swapchain_image_index].signal_semaphore;
    present_info.swapchainCount = 1u;
    present_info.pSwapchains = &m_swapchain;
    present_info.pImageIndices = &m_swapchain_image_index;
    present_info.pResults = nullptr;

    VkPresentId2KHR present_id2{ VK_STRUCTURE_TYPE_PRESENT_ID_2_KHR };
    if (m_present_id2_enabled)
    {
        present_id2.swapchainCount = 1u;
        present_id2.pPresentIds = &m_present_id;
        present_info.pNext = &present_id2;
    }

    // Synchronization happens on GPU only.
    result = vkQueuePresentKHR(m_queue, &present_info);
    if (result != VK_SUCCESS)
    {
        return false;
    }

    // Switch always to next frame.

    m_frame_index = (m_frame_index + 1u) % m_frame_resources.size();

    return true;
}

uint64_t VulkanWindow::getPresentId() const
{
    return m_present_id;
}

VkResult VulkanWindow::waitForPresent(uint64_t present_id, uint64_t timeout) const
{
    VkPresentWait2InfoKHR present_wait2_info{ VK_STRUCTURE_TYPE_PRESENT_WAIT_2_INFO_KHR };
    present_wait2_info.presentId = present_id;
    present_wait2_info.timeout = timeout;

    return vkWaitForPresent2KHR(m_device, m_swapchain, &present_wait2_info);
}

void VulkanWindow::terminate()
{
    if (m_device != VK_NULL_HANDLE)
    {
        vkDeviceWaitIdle(m_device);

        for (auto& frame_resource : m_frame_resources)
        {
            if (frame_resource.wait_semaphore != VK_NULL_HANDLE)
            {
                vkDestroySemaphore(m_device, frame_resource.wait_semaphore, nullptr);
            }

            if (frame_resource.fence != VK_NULL_HANDLE)
            {
                vkDestroyFence(m_device, frame_resource.fence, nullptr);
            }

            if (frame_resource.command_buffer != VK_NULL_HANDLE)
            {
                vkFreeCommandBuffers(m_device, m_command_pool, 1u, &frame_resource.command_buffer);
            }
        }
        m_frame_resources.clear();
        m_frame_index = 0u;

        for (auto& image_resource : m_swapchain_image_resources)
        {
            if (image_resource.signal_semaphore != VK_NULL_HANDLE)
            {
                vkDestroySemaphore(m_device, image_resource.signal_semaphore, nullptr);
            }

            if (image_resource.image_view != VK_NULL_HANDLE)
            {
                vkDestroyImageView(m_device, image_resource.image_view, nullptr);
            }

            // No need to destroy image
        }
        m_swapchain_image_resources.clear();
        m_swapchain_image_index = 0u;

        if (m_command_pool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(m_device, m_command_pool, nullptr);
            m_command_pool = VK_NULL_HANDLE;
        }

        if (m_swapchain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
            m_swapchain = VK_NULL_HANDLE;
        }

        //

        destroyResource(m_device, m_depth_stencil_image_resource);
        m_depth_stencil_image_barrier_index = -1;

        destroyResource(m_device, m_msaa_image_resource);
        m_msaa_image_barrier_index = -1;
    }

    m_current_extent = { 0u, 0u };

    //

    m_render_image_memory_barriers.clear();
    m_present_image_memory_barriers.clear();

    //

    m_queue = VK_NULL_HANDLE;
}
