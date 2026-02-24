#include "vulkan_stage.h"

#include "gpu/vulkan/builder/vulkan_device_memory.h"
#include "gpu/vulkan/factory/VulkanBufferFactory.h"

std::optional<VulkanBufferResource> createStagingBuffer(VkPhysicalDevice physical_device, VkDevice device, VkDeviceSize buffer_size)
{
    VulkanBufferResource buffer_resource{};

    VulkanBufferFactory buffer_factory{ device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT };
    buffer_resource.buffer = buffer_factory.create();
    if (buffer_resource.buffer == VK_NULL_HANDLE)
    {
        destroyResource(device, buffer_resource);

        return {};
    }

    buffer_resource.device_memory = buildBufferDeviceMemory(physical_device, device, buffer_resource.buffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    if (buffer_resource.device_memory == VK_NULL_HANDLE)
    {
        destroyResource(device, buffer_resource);

        return {};
    }

    return buffer_resource;
}

void copyBufferToImage(VkCommandBuffer command_buffer, VkBuffer src_buffer, VkImage dst_image, VkExtent3D extent, VkImageSubresourceLayers subresource_layers)
{
    VkBufferImageCopy2 buffer_image_copy{ VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2 };
    buffer_image_copy.bufferOffset = 0u;
    buffer_image_copy.bufferRowLength = 0u;
    buffer_image_copy.bufferImageHeight = 0u;
    buffer_image_copy.imageSubresource = subresource_layers;
    buffer_image_copy.imageOffset = { 0, 0, 0 };
    buffer_image_copy.imageExtent = extent;

    VkCopyBufferToImageInfo2 copy_buffer_to_image_info{ VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2 };
    copy_buffer_to_image_info.srcBuffer = src_buffer;
    copy_buffer_to_image_info.dstImage = dst_image;
    copy_buffer_to_image_info.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    copy_buffer_to_image_info.regionCount = 1u;
    copy_buffer_to_image_info.pRegions = &buffer_image_copy;

    vkCmdCopyBufferToImage2(command_buffer, &copy_buffer_to_image_info);
}

void copyImageToBuffer(VkCommandBuffer command_buffer, VkImage src_image, VkBuffer dst_buffer, VkExtent3D extent, VkImageSubresourceLayers subresource_layers)
{
    VkBufferImageCopy2 buffer_image_copy{ VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2 };
    buffer_image_copy.bufferOffset = 0u;
    buffer_image_copy.bufferRowLength = 0u;
    buffer_image_copy.bufferImageHeight = 0u;
    buffer_image_copy.imageSubresource = subresource_layers;
    buffer_image_copy.imageOffset = { 0, 0, 0 };
    buffer_image_copy.imageExtent = extent;

    VkCopyImageToBufferInfo2 copy_image_to_buffer_info{ VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2 };
    copy_image_to_buffer_info.srcImage = src_image;
    copy_image_to_buffer_info.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    copy_image_to_buffer_info.dstBuffer = dst_buffer;
    copy_image_to_buffer_info.regionCount = 1u;
    copy_image_to_buffer_info.pRegions = &buffer_image_copy;

    vkCmdCopyImageToBuffer2(command_buffer, &copy_image_to_buffer_info);
}

void transitionImageLayout(VkCommandBuffer command_buffer, VkImage image, VkImageLayout old_layout, VkImageLayout new_layout, VkImageAspectFlags aspect_mask, uint32_t base_mip_level, uint32_t level_count, uint32_t layer_count)
{
    VkImageMemoryBarrier2 image_memory_barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
    image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.image = image;
    image_memory_barrier.subresourceRange = { aspect_mask, base_mip_level, level_count, 0u, layer_count };
    image_memory_barrier.oldLayout = old_layout;
    image_memory_barrier.newLayout = new_layout;

    // Determine stage and access masks based on layouts
    // Using specific stages instead of ALL_COMMANDS for better performance
    switch (old_layout)
    {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            image_memory_barrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
            image_memory_barrier.srcAccessMask = 0u;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            image_memory_barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
            image_memory_barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            image_memory_barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
            image_memory_barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
            break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            image_memory_barrier.srcStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
            image_memory_barrier.srcAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
            break;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            image_memory_barrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
            image_memory_barrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
            break;
        default:
            image_memory_barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
            image_memory_barrier.srcAccessMask = VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;
            break;
    }

    switch (new_layout)
    {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            image_memory_barrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
            image_memory_barrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            image_memory_barrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
            image_memory_barrier.dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
            break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            image_memory_barrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
            image_memory_barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
            break;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            image_memory_barrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
            image_memory_barrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            image_memory_barrier.dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
            image_memory_barrier.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
            image_memory_barrier.dstStageMask = VK_PIPELINE_STAGE_2_NONE;
            image_memory_barrier.dstAccessMask = 0u;
            break;
        default:
            image_memory_barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
            image_memory_barrier.dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;
            break;
    }

    VkDependencyInfo dependency_info{ VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
    dependency_info.imageMemoryBarrierCount = 1u;
    dependency_info.pImageMemoryBarriers = &image_memory_barrier;

    vkCmdPipelineBarrier2(command_buffer, &dependency_info);
}

void copyHostToImage(VkDevice device, const void* src_data, uint32_t src_row_length, uint32_t src_image_height, VkImage dst_image, VkImageLayout dst_image_layout, VkExtent3D extent, VkImageSubresourceLayers subresource_layers)
{
    VkMemoryToImageCopy memory_to_image_copy{ VK_STRUCTURE_TYPE_MEMORY_TO_IMAGE_COPY };
    memory_to_image_copy.pHostPointer = src_data;
    memory_to_image_copy.memoryRowLength = src_row_length;
    memory_to_image_copy.memoryImageHeight = src_image_height;
    memory_to_image_copy.imageSubresource = subresource_layers;
    memory_to_image_copy.imageOffset = { 0, 0, 0 };
    memory_to_image_copy.imageExtent = extent;

    VkCopyMemoryToImageInfo copy_info{ VK_STRUCTURE_TYPE_COPY_MEMORY_TO_IMAGE_INFO };
    copy_info.dstImage = dst_image;
    copy_info.dstImageLayout = dst_image_layout;
    copy_info.regionCount = 1u;
    copy_info.pRegions = &memory_to_image_copy;

    vkCopyMemoryToImage(device, &copy_info);
}

void hostTransitionImageLayout(VkDevice device, VkImage image, VkImageLayout old_layout, VkImageLayout new_layout, VkImageAspectFlags aspect_mask, uint32_t base_mip_level, uint32_t level_count, uint32_t layer_count)
{
    VkHostImageLayoutTransitionInfo transition{ VK_STRUCTURE_TYPE_HOST_IMAGE_LAYOUT_TRANSITION_INFO };
    transition.image = image;
    transition.oldLayout = old_layout;
    transition.newLayout = new_layout;
    transition.subresourceRange = { aspect_mask, base_mip_level, level_count, 0u, layer_count };

    vkTransitionImageLayout(device, 1u, &transition);
}
