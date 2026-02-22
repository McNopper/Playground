#include "Texture2D.h"

#include "gpu/gpu.h"

Texture2D::Texture2D(VkPhysicalDevice physical_device, VkDevice device) :
    Texture(physical_device, device)
{
    m_image_type = VK_IMAGE_TYPE_2D;
    m_image_view_type = VK_IMAGE_VIEW_TYPE_2D;
    m_extent.depth = 1u;
}

Texture2D::~Texture2D()
{
}

void Texture2D::setWidth(uint32_t width)
{
    m_extent.width = width;
}

void Texture2D::setHeight(uint32_t height)
{
    m_extent.height = height;
}

void Texture2D::setExtent(uint32_t width, uint32_t height)
{
    m_extent.width = width;
    m_extent.height = height;
    m_extent.depth = 1u;
}

bool Texture2D::create()
{
    return createImage();
}

bool Texture2D::upload(VkCommandPool command_pool, VkQueue queue, const ImageData& image_data, uint32_t mip_level)
{
    if (!isValid())
    {
        return false;
    }

    uint32_t expected_width  = std::max(m_extent.width  >> mip_level, 1u);
    uint32_t expected_height = std::max(m_extent.height >> mip_level, 1u);
    if (image_data.width != expected_width || image_data.height != expected_height)
    {
        return false;
    }

    VkFormat expected_format = getVulkanFormat(image_data);
    if (expected_format != m_format)
    {
        return false;
    }

    if (mip_level >= m_mip_levels)
    {
        return false;
    }

    // Calculate buffer size
    VkDeviceSize buffer_size = static_cast<VkDeviceSize>(image_data.pixels.size());

    // Create staging buffer
    auto staging_buffer = createStagingBuffer(m_physical_device, m_device, buffer_size);
    if (!staging_buffer.has_value())
    {
        return false;
    }

    // Copy image data to staging buffer
    if (!hostToDevice(m_device, staging_buffer->device_memory, 0u, buffer_size, image_data.pixels))
    {
        destroyResource(m_device, *staging_buffer);
        return false;
    }

    // Create command buffer
    VulkanCommandBufferFactory command_buffer_factory{ m_device, command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1u };
    auto command_buffers = command_buffer_factory.create();
    if (command_buffers.empty())
    {
        destroyResource(m_device, *staging_buffer);
        return false;
    }
    VkCommandBuffer command_buffer = command_buffers[0];

    // Begin command buffer
    VkCommandBufferBeginInfo begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VkResult result = vkBeginCommandBuffer(command_buffer, &begin_info);
    if (result != VK_SUCCESS)
    {
        destroyResource(m_device, *staging_buffer);
        return false;
    }

    // Transition mip level: UNDEFINED -> TRANSFER_DST_OPTIMAL
    transitionImageLayout(command_buffer, m_image_resource.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, mip_level, 1u);

    // Copy buffer to image
    VkImageSubresourceLayers subresource_layers{};
    subresource_layers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource_layers.mipLevel = mip_level;
    subresource_layers.baseArrayLayer = 0u;
    subresource_layers.layerCount = 1u;

    VkExtent3D mip_extent{ expected_width, expected_height, 1u };
    copyBufferToImage(command_buffer, staging_buffer->buffer, m_image_resource.image, mip_extent, subresource_layers);

    // Transition mip level: TRANSFER_DST_OPTIMAL -> SHADER_READ_ONLY_OPTIMAL
    transitionImageLayout(command_buffer, m_image_resource.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, mip_level, 1u);

    result = vkEndCommandBuffer(command_buffer);
    if (result != VK_SUCCESS)
    {
        destroyResource(m_device, *staging_buffer);
        return false;
    }

    // Submit command buffer (Vulkan 1.3)
    VkCommandBufferSubmitInfo cmd_buffer_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
    cmd_buffer_info.commandBuffer = command_buffer;

    VkSubmitInfo2 submit_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
    submit_info.commandBufferInfoCount = 1u;
    submit_info.pCommandBufferInfos = &cmd_buffer_info;

    result = vkQueueSubmit2(queue, 1u, &submit_info, VK_NULL_HANDLE);
    if (result != VK_SUCCESS)
    {
        destroyResource(m_device, *staging_buffer);
        return false;
    }

    // Wait for queue to finish
    result = vkQueueWaitIdle(queue);
    if (result != VK_SUCCESS)
    {
        destroyResource(m_device, *staging_buffer);
        return false;
    }

    // Cleanup staging buffer
    destroyResource(m_device, *staging_buffer);

    return true;
}

uint32_t Texture2D::getWidth() const
{
    return m_extent.width;
}

uint32_t Texture2D::getHeight() const
{
    return m_extent.height;
}

bool Texture2D::uploadMipMaps(VkCommandPool command_pool, VkQueue queue, const std::vector<ImageData>& mip_levels)
{
    if (static_cast<uint32_t>(mip_levels.size()) != m_mip_levels)
    {
        return false;
    }

    for (uint32_t i{ 0u }; i < static_cast<uint32_t>(mip_levels.size()); ++i)
    {
        if (!upload(command_pool, queue, mip_levels[i], i))
        {
            return false;
        }
    }

    return true;
}
