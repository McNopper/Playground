#include "Application.h"

#include <map>
#include <string>
#include <vector>

#include "core/core.h"

Application::Application(VkPhysicalDevice physical_device, VkDevice device, uint32_t queue_family_index, IVulkanWindow& vulkan_window) :
	m_physical_device{ physical_device }, m_device{ device }, m_queue_family_index{ queue_family_index }, m_vulkan_window{ vulkan_window },
	m_vertex_buffer(physical_device, device),
	m_index_buffer(physical_device, device),
	m_uniform_view_buffer(physical_device, device),
	m_uniform_model_buffer(physical_device, device),
	m_texture(physical_device, device),
	m_sampler(device),
	m_descriptor_set0(physical_device, device),
	m_descriptor_set1(physical_device, device)
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
		{{ -0.5f, -0.5f, +0.5f }, { 0.0f, 1.0f }},  // Bottom-left -> UV bottom
		{{ +0.5f, -0.5f, +0.5f }, { 1.0f, 1.0f }},  // Bottom-right -> UV bottom
		{{ +0.5f, +0.5f, +0.5f }, { 1.0f, 0.0f }},  // Top-right -> UV top
		{{ -0.5f, +0.5f, +0.5f }, { 0.0f, 0.0f }},  // Top-left -> UV top
		
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
		
		// Top face (y = +0.5) - looking down
		{{ -0.5f, +0.5f, +0.5f }, { 0.0f, 0.0f }},
		{{ +0.5f, +0.5f, +0.5f }, { 1.0f, 0.0f }},
		{{ +0.5f, +0.5f, -0.5f }, { 1.0f, 1.0f }},
		{{ -0.5f, +0.5f, -0.5f }, { 0.0f, 1.0f }},
		
		// Bottom face (y = -0.5) - looking up
		{{ -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f }},
		{{ +0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f }},
		{{ +0.5f, -0.5f, +0.5f }, { 1.0f, 1.0f }},
		{{ -0.5f, -0.5f, +0.5f }, { 0.0f, 1.0f }}
	};

	// Index buffer for cube (6 faces * 2 triangles * 3 indices = 36 indices)
	std::vector<uint16_t> index_buffer_data{
		// Front
		0, 1, 2,  0, 2, 3,
		// Back
		4, 5, 6,  4, 6, 7,
		// Left
		8, 9, 10,  8, 10, 11,
		// Right
		12, 13, 14,  12, 14, 15,
		// Top
		16, 17, 18,  16, 18, 19,
		// Bottom
		20, 21, 22,  20, 22, 23
	};

	if (!m_vertex_buffer.create(vertex_buffer_data))
	{
		return false;
	}

	if (!m_index_buffer.create(index_buffer_data))
	{
		return false;
	}

	std::map<std::string, std::string> macros{};
	auto shaders = buildSlang("textured_quad.slang", macros, "../resources/shaders/");
	if (shaders.empty())
	{
		return false;
	}

	// Create UniformBlocks from shader reflection
	VulkanSpirvQuery spirv_query{ shaders };
	
	m_uniform_view_block = std::make_shared<UniformBlock>(spirv_query, "UniformViewData");
	m_uniform_model_block = std::make_shared<UniformBlock>(spirv_query, "UniformModelData");

	// Create uniform buffers for descriptor buffer usage
	if (!m_uniform_view_buffer.create(m_uniform_view_block->size()))
	{
		return false;
	}

	if (!m_uniform_model_buffer.create(m_uniform_model_block->size()))
	{
		return false;
	}

	m_descriptor_set0.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	m_descriptor_set0.addBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	
	if (!m_descriptor_set0.create())
	{
		return false;
	}

	VkDescriptorBufferInfo descriptor_buffer_info_view{};
	descriptor_buffer_info_view.buffer = m_uniform_view_buffer.getBuffer();
	descriptor_buffer_info_view.offset = 0u;
	descriptor_buffer_info_view.range = m_uniform_view_block->size();

	if (!m_descriptor_set0.writeBufferDescriptor(0, descriptor_buffer_info_view))
	{
		return false;
	}

	VkDescriptorBufferInfo descriptor_buffer_info_model{};
	descriptor_buffer_info_model.buffer = m_uniform_model_buffer.getBuffer();
	descriptor_buffer_info_model.offset = 0u;
	descriptor_buffer_info_model.range = m_uniform_model_block->size();

	if (!m_descriptor_set0.writeBufferDescriptor(1, descriptor_buffer_info_model))
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

	m_descriptor_set1.addBinding(0, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
	m_descriptor_set1.addBinding(1, VK_DESCRIPTOR_TYPE_SAMPLER);
	
	if (!m_descriptor_set1.create())
	{
		return false;
	}

	VkDescriptorImageInfo descriptor_image_info{};
	descriptor_image_info.sampler = VK_NULL_HANDLE;
	descriptor_image_info.imageView = m_texture.getImageView();
	descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	if (!m_descriptor_set1.writeImageDescriptor(0, descriptor_image_info))
	{
		return false;
	}

	VkDescriptorImageInfo descriptor_sampler_info{};
	descriptor_sampler_info.sampler = m_sampler.getSampler();
	descriptor_sampler_info.imageView = VK_NULL_HANDLE;
	descriptor_sampler_info.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	if (!m_descriptor_set1.writeImageDescriptor(1, descriptor_sampler_info))
	{
		return false;
	}

	VulkanPipelineLayoutFactory pipeline_layout_factory{ m_device, 0u };
	
	VkDescriptorSetLayoutCreateFlags layout_flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_DESCRIPTOR_BUFFER_BIT_EXT;
	
	VulkanDescriptorSetLayoutFactory descriptor_set_layout_factory_0{ m_device, layout_flags };
	
	VkDescriptorSetLayoutBinding uniform_view_binding{};
	uniform_view_binding.binding = 0;
	uniform_view_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniform_view_binding.descriptorCount = 1;
	uniform_view_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uniform_view_binding.pImmutableSamplers = nullptr;
	descriptor_set_layout_factory_0.addDescriptorSetLayoutBinding(uniform_view_binding);
	
	VkDescriptorSetLayoutBinding uniform_model_binding{};
	uniform_model_binding.binding = 1;
	uniform_model_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniform_model_binding.descriptorCount = 1;
	uniform_model_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uniform_model_binding.pImmutableSamplers = nullptr;
	descriptor_set_layout_factory_0.addDescriptorSetLayoutBinding(uniform_model_binding);
	
	VkDescriptorSetLayout descriptor_set_layout_0 = descriptor_set_layout_factory_0.create();
	if (descriptor_set_layout_0 == VK_NULL_HANDLE)
	{
		return false;
	}
	
	VulkanDescriptorSetLayoutFactory descriptor_set_layout_factory_1{ m_device, layout_flags };
	
	VkDescriptorSetLayoutBinding texture_binding{};
	texture_binding.binding = 0;
	texture_binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	texture_binding.descriptorCount = 1;
	texture_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	texture_binding.pImmutableSamplers = nullptr;
	descriptor_set_layout_factory_1.addDescriptorSetLayoutBinding(texture_binding);
	
	VkDescriptorSetLayoutBinding sampler_binding{};
	sampler_binding.binding = 1;
	sampler_binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
	sampler_binding.descriptorCount = 1;
	sampler_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	sampler_binding.pImmutableSamplers = nullptr;
	descriptor_set_layout_factory_1.addDescriptorSetLayoutBinding(sampler_binding);
	
	VkDescriptorSetLayout descriptor_set_layout_1 = descriptor_set_layout_factory_1.create();
	if (descriptor_set_layout_1 == VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(m_device, descriptor_set_layout_0, nullptr);
		return false;
	}

	pipeline_layout_factory.addDescriptorSetLayout(descriptor_set_layout_0);
	pipeline_layout_factory.addDescriptorSetLayout(descriptor_set_layout_1);
	m_pipeline_layout = pipeline_layout_factory.create();
	
	vkDestroyDescriptorSetLayout(m_device, descriptor_set_layout_0, nullptr);
	vkDestroyDescriptorSetLayout(m_device, descriptor_set_layout_1, nullptr);
	
	if (m_pipeline_layout == VK_NULL_HANDLE)
	{
		return false;
	}

	VulkanGraphicsPipelineBuilder graphics_pipeline_builder{ m_device, VK_PIPELINE_CREATE_DESCRIPTOR_BUFFER_BIT_EXT, m_pipeline_layout };

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

	m_pipeline = graphics_pipeline_builder.create(shaders);
	if (m_pipeline == VK_NULL_HANDLE)
	{
		return false;
	}

	m_vulkan_window.setClearColor(0.25f, 0.25f, 0.25f, 1.0f);

	return true;
}

bool Application::update(double delta_time, VkCommandBuffer command_buffer)
{
	m_rotation_angle += delta_time * 45.0;

	// Update uniform data using UniformBlock
	float aspect = (float)m_vulkan_window.getCurrentExtent().width / (float)m_vulkan_window.getCurrentExtent().height;
	float4x4 projection_matrix = perspective(45.0f, aspect, 0.1f, 100.0f);
	float4x4 view_matrix = lookAt(float3{ 0.0f, 1.0f, 3.0f }, float3{ 0.0f, 0.0f, 0.0f }, float3{ 0.0f, 1.0f, 0.0f });
	
	m_uniform_view_block->setMember("u_projectionMatrix", projection_matrix);
	m_uniform_view_block->setMember("u_viewMatrix", view_matrix);

	if (!m_uniform_view_buffer.update(0u, m_uniform_view_block->getData()))
	{
		return false;
	}

	quaternion rot_y = rotateRyQuaternion((float)m_rotation_angle);
	float4x4 world_matrix = rot_y;
	
	m_uniform_model_block->setMember("u_worldMatrix", world_matrix);

	if (!m_uniform_model_buffer.update(0u, m_uniform_model_block->getData()))
	{
		return false;
	}

	m_vulkan_window.beginRendering();

	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

	VkDescriptorBufferBindingInfoEXT descriptor_buffer_bindings[2]{};
	
	descriptor_buffer_bindings[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT;
	descriptor_buffer_bindings[0].address = m_descriptor_set0.getDeviceAddress();
	descriptor_buffer_bindings[0].usage = m_descriptor_set0.getUsageFlags();
	
	descriptor_buffer_bindings[1].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT;
	descriptor_buffer_bindings[1].address = m_descriptor_set1.getDeviceAddress();
	descriptor_buffer_bindings[1].usage = m_descriptor_set1.getUsageFlags();

	vkCmdBindDescriptorBuffersEXT(command_buffer, 2, descriptor_buffer_bindings);

	uint32_t buffer_index_set0 = 0;
	VkDeviceSize descriptor_offset_set0 = 0;
	vkCmdSetDescriptorBufferOffsetsEXT(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_layout, 0, 1, &buffer_index_set0, &descriptor_offset_set0);
	
	uint32_t buffer_index_set1 = 1;
	VkDeviceSize descriptor_offset_set1 = 0;
	vkCmdSetDescriptorBufferOffsetsEXT(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_layout, 1, 1, &buffer_index_set1, &descriptor_offset_set1);

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

	VkDeviceSize offset{ 0u };
	VkBuffer vertex_buffer = m_vertex_buffer.getBuffer();
	vkCmdBindVertexBuffers(command_buffer, 0u, 1u, &vertex_buffer, &offset);

	VkBuffer index_buffer = m_index_buffer.getBuffer();
	vkCmdBindIndexBuffer(command_buffer, index_buffer, 0u, VK_INDEX_TYPE_UINT16);

	vkCmdDrawIndexed(command_buffer, 36u, 1u, 0u, 0, 0u);

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

	if (m_pipeline_layout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
		m_pipeline_layout = VK_NULL_HANDLE;
	}

	m_descriptor_set0.destroy();
	m_descriptor_set1.destroy();
	m_vertex_buffer.destroy();
	m_index_buffer.destroy();
	m_uniform_view_buffer.destroy();
	m_uniform_model_buffer.destroy();
	m_texture.destroy();
	m_sampler.destroy();
}
