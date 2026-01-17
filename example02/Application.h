#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <cstdint>

#include <volk.h>

#include "gpu/gpu.h"
#include "engine/engine.h"

/**
 * Application - Compute shader application
 * 
 * Generates a grey square image using compute shader:
 *   - Storage buffer for output
 *   - Descriptor buffer with storage buffer descriptor
 *   - Compute pipeline
 *   - Saves result as EXR image file
 */

class Application : public IApplication
{

private:

	const uint32_t c_dimension{ 128u };
	const VkDeviceSize c_buffer_size{ sizeof(float) * c_dimension * c_dimension };

	VkPhysicalDevice m_physical_device{ VK_NULL_HANDLE };
	VkDevice m_device{ VK_NULL_HANDLE };
	uint32_t m_queue_family_index{ VK_QUEUE_FAMILY_IGNORED };

	VkQueue m_queue{ VK_NULL_HANDLE };

	VkPipelineLayout m_pipeline_layout{ VK_NULL_HANDLE };
	VkPipeline m_pipeline{ VK_NULL_HANDLE };

	StorageBuffer m_output_storage_buffer;
	DescriptorBuffer m_descriptor_buffer;

public:

	Application(VkPhysicalDevice physical_device, VkDevice device, uint32_t queue_family_index);

	bool init();

	bool update(double delta_time, VkCommandBuffer command_buffer) override;

	void terminate();

};

#endif /* APPLICATION_H_ */
