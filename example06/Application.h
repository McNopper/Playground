#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <memory>

#include <volk.h>

#include "gpu/gpu.h"
#include "engine/engine.h"

struct UniformViewData
{
	float4x4 u_projectionMatrix{ 1.0f };
	float4x4 u_viewMatrix{ 1.0f };
};

struct UniformModelData
{
	float4x4 u_worldMatrix{ 1.0f };
};

class Application : public IApplication
{

private:

	VkPhysicalDevice m_physical_device{ VK_NULL_HANDLE };
	VkDevice m_device{ VK_NULL_HANDLE };
	uint32_t m_queue_family_index{ VK_QUEUE_FAMILY_IGNORED };

	IVulkanWindow& m_vulkan_window;

	// Geometry and material - using new classes
	std::unique_ptr<TriangleMesh> m_mesh;
	std::unique_ptr<MaterialShader> m_material;

	// Resources
	VertexBuffer m_vertex_buffer;
	IndexBuffer m_index_buffer;
	UniformBuffer m_uniform_view_buffer;
	UniformBuffer m_uniform_model_buffer;
	Texture2D m_texture;
	Sampler m_sampler;

	VkPipeline m_pipeline{ VK_NULL_HANDLE };

	double m_rotation_angle{ 0.0 };

public:

	Application(VkPhysicalDevice physical_device, VkDevice device, uint32_t queue_family_index, IVulkanWindow& vulkan_window);

	bool init() override;

	bool update(double delta_time, VkCommandBuffer command_buffer) override;

	void terminate() override;

};

#endif /* APPLICATION_H_ */
