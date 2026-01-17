#ifndef RENDERER_BACKEND_COMMON_BUFFER_DESCRIPTORBUFFERSET_H_
#define RENDERER_BACKEND_COMMON_BUFFER_DESCRIPTORBUFFERSET_H_

#include <vector>
#include <map>

#include <volk.h>

#include "GpuBuffer.h"

// DescriptorBufferSet - Enhanced descriptor buffer supporting multiple descriptor types
// 
// Unlike DescriptorBuffer which only supports one type, this class allows multiple
// descriptor types in a single buffer, managing proper alignment and offsets.
//
// Usage:
// 1. Add descriptor bindings with their types
// 2. Create the buffer (calculates sizes and offsets)
// 3. Write individual descriptors by binding index
// 4. Use getBindingOffset() when setting descriptor buffer offsets
//
// Example:
//   DescriptorBufferSet desc_set(physical_device, device);
//   desc_set.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
//   desc_set.addBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
//   desc_set.addBinding(2, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
//   desc_set.addBinding(3, VK_DESCRIPTOR_TYPE_SAMPLER);
//   desc_set.create();
//   desc_set.writeBufferDescriptor(0, buffer_info);
//   desc_set.writeImageDescriptor(2, image_info);

struct DescriptorBinding {
	uint32_t binding{ 0u };
	VkDescriptorType type{ VK_DESCRIPTOR_TYPE_MAX_ENUM };
	VkDeviceSize offset{ 0u };
	VkDeviceSize size{ 0u };
};

class DescriptorBufferSet : public GpuBuffer {

private:

	std::map<uint32_t, DescriptorBinding> m_bindings{};
	VkDeviceAddress m_device_address{ 0u };
	VkDeviceSize m_alignment{ 0u };
	VkBufferUsageFlags m_usage{ 0u };
	
	bool queryDescriptorSizes();
	VkDeviceSize getDescriptorSize(VkDescriptorType type) const;
	VkDeviceSize alignOffset(VkDeviceSize offset) const;

public:

	DescriptorBufferSet() = delete;
	DescriptorBufferSet(const DescriptorBufferSet& other) = delete;

	DescriptorBufferSet(VkPhysicalDevice physical_device, VkDevice device);

	~DescriptorBufferSet() override;

	DescriptorBufferSet operator=(const DescriptorBufferSet& other) = delete;

	// Add a binding before calling create()
	bool addBinding(uint32_t binding, VkDescriptorType type);

	// Create the descriptor buffer (call after adding all bindings)
	bool create();

	// Write descriptors by binding index
	bool writeImageDescriptor(uint32_t binding, const VkDescriptorImageInfo& image_info);
	bool writeBufferDescriptor(uint32_t binding, const VkDescriptorBufferInfo& buffer_info);
	bool writeAccelerationStructureDescriptor(uint32_t binding, VkAccelerationStructureKHR acceleration_structure);

	// Get offset for a specific binding (for vkCmdSetDescriptorBufferOffsetsEXT)
	VkDeviceSize getBindingOffset(uint32_t binding) const;
	
	VkDeviceAddress getDeviceAddress() const;
	VkBufferUsageFlags getUsageFlags() const;

};

#endif /* RENDERER_BACKEND_COMMON_BUFFER_DESCRIPTORBUFFERSET_H_ */
