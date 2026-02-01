#include "Application.h"

#include <vector>

#include "core/core.h"

Application::Application(VkPhysicalDevice physical_device, VkDevice device, uint32_t queue_family_index, IVulkanWindow& vulkan_window) :
	m_physical_device{ physical_device }, m_device{ device }, m_queue_family_index{ queue_family_index }, m_vulkan_window{ vulkan_window },
	m_vertex_buffer(physical_device, device),
	m_index_buffer(physical_device, device),
	m_uniform_view_buffer(physical_device, device),
	m_uniform_model_buffer(physical_device, device),
	m_texture(physical_device, device),
	m_sampler(device)
{
}

bool Application::init()
{
	struct VertexInputData
	{
		float3	position;
		float2	texCoord;
	};

	// Cube vertices (24 vertices for proper texture mapping per face)
	// UV convention: (0,0) = top-left, (1,1) = bottom-right (Vulkan/glTF standard)
	std::vector<VertexInputData> vertex_buffer_data{
		// Front face (z = +0.5)
		{{ -0.5f, -0.5f, +0.5f }, { 0.0f, 1.0f }},
		{{ +0.5f, -0.5f, +0.5f }, { 1.0f, 1.0f }},
		{{ +0.5f, +0.5f, +0.5f }, { 1.0f, 0.0f }},
		{{ -0.5f, +0.5f, +0.5f }, { 0.0f, 0.0f }},
		
		// Back face (z = -0.5)
		{{ +0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f }},
		{{ -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }},
		{{ -0.5f, +0.5f, -0.5f }, { 1.0f, 0.0f }},
		{{ +0.5f, +0.5f, -0.5f }, { 0.0f, 0.0f }},
		
		// Left face (x = -0.5)
		{{ -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f }},
		{{ -0.5f, -0.5f, +0.5f }, { 1.0f, 1.0f }},
		{{ -0.5f, +0.5f, +0.5f }, { 1.0f, 0.0f }},
		{{ -0.5f, +0.5f, -0.5f }, { 0.0f, 0.0f }},
		
		// Right face (x = +0.5)
		{{ +0.5f, -0.5f, +0.5f }, { 0.0f, 1.0f }},
		{{ +0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }},
		{{ +0.5f, +0.5f, -0.5f }, { 1.0f, 0.0f }},
		{{ +0.5f, +0.5f, +0.5f }, { 0.0f, 0.0f }},
		
		// Top face (y = +0.5)
		{{ -0.5f, +0.5f, +0.5f }, { 0.0f, 0.0f }},
		{{ +0.5f, +0.5f, +0.5f }, { 1.0f, 0.0f }},
		{{ +0.5f, +0.5f, -0.5f }, { 1.0f, 1.0f }},
		{{ -0.5f, +0.5f, -0.5f }, { 0.0f, 1.0f }},
		
		// Bottom face (y = -0.5)
		{{ -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f }},
		{{ +0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f }},
		{{ +0.5f, -0.5f, +0.5f }, { 1.0f, 1.0f }},
		{{ -0.5f, -0.5f, +0.5f }, { 0.0f, 1.0f }}
	};

	// Index buffer for cube (6 faces * 2 triangles * 3 indices = 36 indices)
	std::vector<uint16_t> index_buffer_data{
		0, 1, 2,  0, 2, 3,    // Front
		4, 5, 6,  4, 6, 7,    // Back
		8, 9, 10,  8, 10, 11, // Left
		12, 13, 14,  12, 14, 15, // Right
		16, 17, 18,  16, 18, 19, // Top
		20, 21, 22,  20, 22, 23  // Bottom
	};

	if (!m_vertex_buffer.create(vertex_buffer_data))
	{
		return false;
	}

	if (!m_index_buffer.create(index_buffer_data))
	{
		return false;
	}

	if (!m_uniform_view_buffer.create(sizeof(UniformViewData)))
	{
		return false;
	}

	if (!m_uniform_model_buffer.create(sizeof(UniformModelData)))
	{
		return false;
	}

	auto image_data_opt = loadImageData("../resources/images/color_grid.png");
	if (!image_data_opt.has_value())
	{
		return false;
	}

	auto image_data = convertImageDataChannels(4, image_data_opt.value());
	if (!image_data.has_value())
	{
		return false;
	}

	m_texture.setExtent(image_data->width, image_data->height);
	m_texture.setFormat(VK_FORMAT_R8G8B8A8_SRGB);
	m_texture.setUsage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
	m_texture.setMipLevels(1);

	if (!m_texture.create())
	{
		return false;
	}

	VulkanCommandPoolFactory command_pool_factory{ m_device, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, m_queue_family_index };
	VkCommandPool command_pool = command_pool_factory.create();
	if (command_pool == VK_NULL_HANDLE)
	{
		return false;
	}

	VkQueue queue = VK_NULL_HANDLE;
	vkGetDeviceQueue(m_device, m_queue_family_index, 0, &queue);

	if (!m_texture.upload(command_pool, queue, image_data.value()))
	{
		vkDestroyCommandPool(m_device, command_pool, nullptr);
		return false;
	}

	vkDestroyCommandPool(m_device, command_pool, nullptr);

	m_sampler.setMagFilter(VK_FILTER_LINEAR);
	m_sampler.setMinFilter(VK_FILTER_LINEAR);
	m_sampler.setAddressMode(VK_SAMPLER_ADDRESS_MODE_REPEAT);

	if (!m_sampler.create())
	{
		return false;
	}

	// Create MaterialShader - handles shader compilation and descriptor management
	m_material = std::make_unique<MaterialShader>(m_physical_device, m_device, "textured_quad.slang", "../resources/shaders/");
	
	if (!m_material->build())
	{
		return false;
	}

	// Attach resources to material by shader variable names
	m_material->setUniformBuffer("UniformViewData", &m_uniform_view_buffer);
	m_material->setUniformBuffer("UniformModelData", &m_uniform_model_buffer);
	m_material->setTexture("u_texture", &m_texture);
	m_material->setSampler("u_sampler", &m_sampler);

	// Build descriptor buffers from attached resources
	if (!m_material->buildDescriptors())
	{
		return false;
	}

	// Create graphics pipeline using material's layout
	VulkanGraphicsPipelineBuilder graphics_pipeline_builder{ m_device, VK_PIPELINE_CREATE_DESCRIPTOR_BUFFER_BIT_EXT, m_material->getPipelineLayout() };

	VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_state{};
	pipeline_color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	graphics_pipeline_builder.addColorAttachment(m_vulkan_window.getSurfaceFormat().format, pipeline_color_blend_attachment_state);
	graphics_pipeline_builder.setDepthAttachment(m_vulkan_window.getDepthStencilFormat());
	graphics_pipeline_builder.setStencilAttachment(m_vulkan_window.getDepthStencilFormat());
	graphics_pipeline_builder.setDepthTestEnable(VK_TRUE);
	graphics_pipeline_builder.setDepthWriteEnable(VK_TRUE);
	graphics_pipeline_builder.setDepthCompareOp(VK_COMPARE_OP_LESS);
	graphics_pipeline_builder.setRasterizationSamples(m_vulkan_window.getSamples());

	graphics_pipeline_builder.addDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
	graphics_pipeline_builder.addDynamicState(VK_DYNAMIC_STATE_SCISSOR);

	m_pipeline = graphics_pipeline_builder.create(m_material->getSpirvShaders());
	if (m_pipeline == VK_NULL_HANDLE)
	{
		return false;
	}

	// Create TriangleMesh geometry - handles vertex/index binding
	m_mesh = std::make_unique<TriangleMesh>(m_physical_device, m_device);

	// Set vertex attributes matching shader input names
	m_mesh->setVertexAttribute("i_position", &m_vertex_buffer, 0, VK_FORMAT_R32G32B32_SFLOAT, 0, sizeof(VertexInputData));
	m_mesh->setVertexAttribute("i_texcoord", &m_vertex_buffer, 0, VK_FORMAT_R32G32_SFLOAT, sizeof(float3), sizeof(VertexInputData));

	// Set index buffer
	m_mesh->setIndexBuffer(&m_index_buffer, VK_INDEX_TYPE_UINT16, 36);

	if (!m_mesh->isValid())
	{
		return false;
	}

	m_vulkan_window.setClearColor(0.25f, 0.25f, 0.25f, 1.0f);

	return true;
}

bool Application::update(double delta_time, VkCommandBuffer command_buffer)
{
	m_rotation_angle += delta_time * 45.0;

	UniformViewData uniform_view_data{};
	
	float aspect = (float)m_vulkan_window.getCurrentExtent().width / (float)m_vulkan_window.getCurrentExtent().height;
	uniform_view_data.u_projectionMatrix = perspective(45.0f, aspect, 0.1f, 100.0f);
	uniform_view_data.u_viewMatrix = lookAt(float3{ 0.0f, 1.0f, 3.0f }, float3{ 0.0f, 0.0f, 0.0f }, float3{ 0.0f, 1.0f, 0.0f });

	if (!m_uniform_view_buffer.update(0u, uniform_view_data))
	{
		return false;
	}

	UniformModelData uniform_model_data{};
	
	quaternion rot_y = rotateRyQuaternion((float)m_rotation_angle);
	
	uniform_model_data.u_worldMatrix = rot_y;

	if (!m_uniform_model_buffer.update(0u, uniform_model_data))
	{
		return false;
	}

	m_vulkan_window.beginRendering();

	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

	// Material handles all descriptor binding
	m_material->bind(command_buffer);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)m_vulkan_window.getCurrentExtent().width;
	viewport.height = (float)m_vulkan_window.getCurrentExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(command_buffer, 0u, 1u, &viewport);

	VkRect2D scissor{};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent = m_vulkan_window.getCurrentExtent();
	vkCmdSetScissor(command_buffer, 0u, 1u, &scissor);

	// Mesh handles vertex/index binding and draw call
	m_mesh->bind(command_buffer);
	m_mesh->draw(command_buffer);

	m_vulkan_window.endRendering();

	return true;
}

void Application::terminate()
{
	vkDeviceWaitIdle(m_device);

	if (m_pipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(m_device, m_pipeline, nullptr);
		m_pipeline = VK_NULL_HANDLE;
	}

	m_mesh.reset();
	m_material.reset();
	
	m_vertex_buffer.destroy();
	m_index_buffer.destroy();
	m_uniform_view_buffer.destroy();
	m_uniform_model_buffer.destroy();
	m_texture.destroy();
	m_sampler.destroy();
}
