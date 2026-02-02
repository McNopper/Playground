#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <vector>

#include <volk.h>

#include "gpu/gpu.h"
#include "engine/engine.h"

struct UniformViewData
{
	float4x4 u_projection_matrix{ 1.0f };
	float4x4 u_view_matrix{ 1.0f };
};

struct UniformModelData
{
	float4x4 u_world_matrix{ 1.0f };
};

class Application : public IApplication
{

private:

	VkPhysicalDevice m_physical_device{ VK_NULL_HANDLE };
	VkDevice m_device{ VK_NULL_HANDLE };
	uint32_t m_queue_family_index{ VK_QUEUE_FAMILY_IGNORED };

	IVulkanWindow& m_vulkan_window;

	VertexBuffer m_vertex_buffer;
	IndexBuffer m_index_buffer;

	VkPipelineLayout m_pipeline_layout{ VK_NULL_HANDLE };
	VkPipeline m_pipeline{ VK_NULL_HANDLE };

	UniformBuffer m_uniform_view_buffer;
	UniformBuffer m_uniform_model_buffer;
	Texture2D m_texture;
	Sampler m_sampler;
	DescriptorBufferSet m_descriptor_set0;
	DescriptorBufferSet m_descriptor_set1;

	double m_rotation_angle{ 0.0 };

public:

	Application(VkPhysicalDevice physical_device, VkDevice device, uint32_t queue_family_index, IVulkanWindow& vulkan_window);

	bool init() override;

	bool update(double delta_time, VkCommandBuffer command_buffer) override;

	void terminate() override;

};

#endif /* APPLICATION_H_ */
