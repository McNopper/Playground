#ifndef RENDERER_BACKEND_COMMON_BUFFER_DESCRIPTORBUFFER_H_
#define RENDERER_BACKEND_COMMON_BUFFER_DESCRIPTORBUFFER_H_

#include <vector>

#include <volk.h>

#include "GpuBuffer.h"

// DescriptorBuffer implements VK_EXT_descriptor_buffer extension.
// Descriptors are stored in a GPU buffer instead of traditional descriptor sets.
//
// Requirements for buffers used with descriptor buffers:
// - Uniform/Storage buffers: Must have VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
// - Images: Must have appropriate image usage flags
//
// Usage:
// 1. Create descriptor buffer with descriptor count and type
// 2. Write descriptors using writeBufferDescriptor() or writeImageDescriptor()
// 3. Bind in command buffer with vkCmdBindDescriptorBuffersEXT()
// 4. Set offsets with vkCmdSetDescriptorBufferOffsetsEXT()
class DescriptorBuffer : public GpuBuffer
{

private:

    VkDescriptorType m_descriptor_type{ VK_DESCRIPTOR_TYPE_MAX_ENUM };
    VkDeviceAddress m_device_address{ 0u };
    VkDeviceSize m_descriptor_size{ 0u };

    bool queryDescriptorSize();

public:

    DescriptorBuffer() = delete;
    DescriptorBuffer(const DescriptorBuffer& other) = delete;

    DescriptorBuffer(VkPhysicalDevice physical_device, VkDevice device);

    ~DescriptorBuffer() override;

    DescriptorBuffer operator=(const DescriptorBuffer& other) = delete;

    bool create(VkDeviceSize descriptor_count, VkDescriptorType descriptor_type);

    // Write descriptors to the descriptor buffer.
    // Note: Buffers passed to writeBufferDescriptor() must have been created
    // with VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT to allow vkGetBufferDeviceAddress().
    bool writeImageDescriptor(VkDeviceSize index, const VkDescriptorImageInfo& image_info);
    bool writeBufferDescriptor(VkDeviceSize index, const VkDescriptorBufferInfo& buffer_info);
    bool writeAccelerationStructureDescriptor(VkDeviceSize index, VkAccelerationStructureKHR acceleration_structure);

    VkDeviceAddress getDeviceAddress() const;
    VkDeviceSize getDescriptorSize() const;
    VkDescriptorType getDescriptorType() const;
};

#endif /* RENDERER_BACKEND_COMMON_BUFFER_DESCRIPTORBUFFER_H_ */
