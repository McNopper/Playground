#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <memory>

#include <volk.h>

#include "gpu/gpu.h"
#include "engine/engine.h"

class Application : public IApplication
{

private:

	VkPhysicalDevice m_physical_device{ VK_NULL_HANDLE };
	VkDevice m_device{ VK_NULL_HANDLE };
	uint32_t m_queue_family_index{ VK_QUEUE_FAMILY_IGNORED };

	IVulkanWindow& m_vulkan_window;

	// Scene object - combines geometry, material, and transform
	std::shared_ptr<Renderable> m_renderable;

	// Shared resources (potentially reusable across multiple renderables)
	std::shared_ptr<VertexBuffer> m_vertex_buffer;
	std::shared_ptr<IndexBuffer> m_index_buffer;
	std::shared_ptr<UniformBuffer> m_uniform_view_buffer;
	std::shared_ptr<Texture2D> m_texture;
	std::shared_ptr<Sampler> m_sampler;
	std::shared_ptr<Texture2DSampler> m_texture_sampler;

	VkPipeline m_pipeline{ VK_NULL_HANDLE };

	double m_rotation_angle{ 0.0 };

public:

	Application(VkPhysicalDevice physical_device, VkDevice device, uint32_t queue_family_index, IVulkanWindow& vulkan_window);

	bool init() override;

	bool update(double delta_time, VkCommandBuffer command_buffer) override;

	void terminate() override;

};

#endif /* APPLICATION_H_ */
