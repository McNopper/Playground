#include "Application.h"

#include <map>
#include <string>
#include <vector>

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
	// Only position and texture coordinate information, having the same structure in the shader.

	struct VertexInputData
	{
		float3	position;
		float2	texCoord;
	};

	// Quad with two triangles using 4 vertices
	// Counter-clockwise winding order
	// UV convention: (0,0) = top-left, (1,1) = bottom-right (Vulkan/glTF standard)
	// Positions in Vulkan clip space: Y=-1 is top, Y=+1 is bottom

	std::vector<VertexInputData> vertex_buffer_data{
		{{ -0.5f, +0.5f, 0.0f }, { 0.0f, 1.0f }},  // Bottom-left (Y=+0.5 → bottom)
		{{ +0.5f, +0.5f, 0.0f }, { 1.0f, 1.0f }},  // Bottom-right
		{{ +0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f }},  // Top-right (Y=-0.5 → top)
		{{ -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f }}   // Top-left
	};

	// Index buffer for two triangles making a quad
	// Counter-clockwise winding order
	std::vector<uint16_t> index_buffer_data{
		0, 1, 2,  // First triangle
		0, 2, 3   // Second triangle
	};

	// Create vertex buffer using template method
	if (!m_vertex_buffer.create(vertex_buffer_data))
	{
		return false;
	}

	// Create index buffer using template method
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
	// UniformBuffer automatically includes VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
	if (!m_uniform_view_buffer.create(m_uniform_view_block->size()))
	{
		return false;
	}

	if (!m_uniform_model_buffer.create(m_uniform_model_block->size()))
	{
		return false;
	}

	// Load and create texture
	auto image_data_opt = loadImageData("../resources/images/color_grid.png");
	if (!image_data_opt.has_value())
	{
		return false;
	}

	// Convert to 4 channels (RGBA) if necessary
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

	// Create temporary command pool for texture upload
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

	// Create sampler
	m_sampler.setMagFilter(VK_FILTER_LINEAR);
	m_sampler.setMinFilter(VK_FILTER_LINEAR);
	m_sampler.setAddressMode(VK_SAMPLER_ADDRESS_MODE_REPEAT);

	if (!m_sampler.create())
	{
		return false;
	}

	//
	// Until here, data was created. Now preparing the graphics pipeline.
	//

	//
	// Create descriptor buffers using DescriptorBufferSet for both sets
	//

	// Set 0: 2 uniform buffers (bindings 0, 1)
	m_descriptor_set0.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	m_descriptor_set0.addBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	
	if (!m_descriptor_set0.create())
	{
		return false;
	}

	// Write uniform buffer descriptors
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

	// Set 1: Separated texture and sampler (binding 0 = texture, binding 1 = sampler)
	m_descriptor_set1.addBinding(0, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
	m_descriptor_set1.addBinding(1, VK_DESCRIPTOR_TYPE_SAMPLER);
	
	if (!m_descriptor_set1.create())
	{
		return false;
	}

	// Write texture descriptor (binding 0)
	VkDescriptorImageInfo descriptor_image_info{};
	descriptor_image_info.sampler = VK_NULL_HANDLE;
	descriptor_image_info.imageView = m_texture.getImageView();
	descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	if (!m_descriptor_set1.writeImageDescriptor(0, descriptor_image_info))
	{
		return false;
	}

	// Write sampler descriptor (binding 1)
	VkDescriptorImageInfo descriptor_sampler_info{};
	descriptor_sampler_info.sampler = m_sampler.getSampler();
	descriptor_sampler_info.imageView = VK_NULL_HANDLE;
	descriptor_sampler_info.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	if (!m_descriptor_set1.writeImageDescriptor(1, descriptor_sampler_info))
	{
		return false;
	}

	//
	// Pipeline layout creation with descriptor buffer support - use two sets
	//

	VulkanPipelineLayoutFactory pipeline_layout_factory{ m_device, 0u };
	
	VkDescriptorSetLayoutCreateFlags layout_flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_DESCRIPTOR_BUFFER_BIT_EXT;
	
	// Create descriptor set layout 0 for uniform buffers (bindings 0, 1)
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
	
	// Create descriptor set layout 1 for separated texture and sampler
	VulkanDescriptorSetLayoutFactory descriptor_set_layout_factory_1{ m_device, layout_flags };
	
	// Binding 0: Sampled image (texture)
	VkDescriptorSetLayoutBinding texture_binding{};
	texture_binding.binding = 0;
	texture_binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	texture_binding.descriptorCount = 1;
	texture_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	texture_binding.pImmutableSamplers = nullptr;
	descriptor_set_layout_factory_1.addDescriptorSetLayoutBinding(texture_binding);
	
	// Binding 1: Sampler
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
	
	// Clean up temporary descriptor set layouts
	vkDestroyDescriptorSetLayout(m_device, descriptor_set_layout_0, nullptr);
	vkDestroyDescriptorSetLayout(m_device, descriptor_set_layout_1, nullptr);
	
	if (m_pipeline_layout == VK_NULL_HANDLE)
	{
		return false;
	}

	// Finally create the graphics pipeline with descriptor buffer support.
	VulkanGraphicsPipelineBuilder graphics_pipeline_builder{ m_device, VK_PIPELINE_CREATE_DESCRIPTOR_BUFFER_BIT_EXT, m_pipeline_layout };

	VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_state{};
	pipeline_color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	graphics_pipeline_builder.addColorAttachment(m_vulkan_window.getSurfaceFormat().format, pipeline_color_blend_attachment_state);
	graphics_pipeline_builder.setDepthAttachment(m_vulkan_window.getDepthStencilFormat());
	graphics_pipeline_builder.setStencilAttachment(m_vulkan_window.getDepthStencilFormat());
	graphics_pipeline_builder.setRasterizationSamples(m_vulkan_window.getSamples());

	graphics_pipeline_builder.addDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
	graphics_pipeline_builder.addDynamicState(VK_DYNAMIC_STATE_SCISSOR);

	m_pipeline = graphics_pipeline_builder.create(shaders);
	if (m_pipeline == VK_NULL_HANDLE)
	{
		return false;
	}

	// One time setups.

	m_vulkan_window.setClearColor(0.25f, 0.25f, 0.25f, 1.0f);

	return true;
}

bool Application::update(double delta_time, VkCommandBuffer command_buffer)
{
	// Update uniform data using UniformBlock
	m_uniform_view_block->setMember("u_projectionMatrix", float4x4{ 1.0f });
	m_uniform_view_block->setMember("u_viewMatrix", float4x4{ 1.0f });

	if (!m_uniform_view_buffer.update(0u, m_uniform_view_block->getData()))
	{
		return false;
	}

	m_uniform_model_block->setMember("u_worldMatrix", float4x4{ 1.0f });

	if (!m_uniform_model_buffer.update(0u, m_uniform_model_block->getData()))
	{
		return false;
	}

	// Possible to use the command buffer for tasks before rendering to the current frame.

	// Our own start of rendering.
	m_vulkan_window.beginRendering();

	// Bind and use the graphics pipeline.
	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

	// Bind the descriptor buffers using VK_EXT_descriptor_buffer
	VkDescriptorBufferBindingInfoEXT descriptor_buffer_bindings[2]{};
	
	// Buffer 0: Set 0 (Uniform buffers) - usage flags auto-detected
	descriptor_buffer_bindings[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT;
	descriptor_buffer_bindings[0].address = m_descriptor_set0.getDeviceAddress();
	descriptor_buffer_bindings[0].usage = m_descriptor_set0.getUsageFlags();
	
	// Buffer 1: Set 1 (Combined image sampler) - usage flags auto-detected
	descriptor_buffer_bindings[1].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT;
	descriptor_buffer_bindings[1].address = m_descriptor_set1.getDeviceAddress();
	descriptor_buffer_bindings[1].usage = m_descriptor_set1.getUsageFlags();

	vkCmdBindDescriptorBuffersEXT(command_buffer, 2, descriptor_buffer_bindings);

	// Set descriptor buffer offsets for set 0 (buffer index 0, offset 0)
	uint32_t buffer_index_set0 = 0;
	VkDeviceSize descriptor_offset_set0 = 0;
	vkCmdSetDescriptorBufferOffsetsEXT(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_layout, 0, 1, &buffer_index_set0, &descriptor_offset_set0);
	
	// Set descriptor buffer offsets for set 1 (buffer index 1, offset 0)
	uint32_t buffer_index_set1 = 1;
	VkDeviceSize descriptor_offset_set1 = 0;
	vkCmdSetDescriptorBufferOffsetsEXT(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_layout, 1, 1, &buffer_index_set1, &descriptor_offset_set1);

	// Set the viewport ...
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)m_vulkan_window.getCurrentExtent().width;
	viewport.height = (float)m_vulkan_window.getCurrentExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(command_buffer, 0u, 1u, &viewport);

	// ... and scissor dynamically.
	VkRect2D scissor{};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent = m_vulkan_window.getCurrentExtent();
	vkCmdSetScissor(command_buffer, 0u, 1u, &scissor);

	// Bind the vertex buffer using getBuffer()
	VkDeviceSize offset{ 0u };
	VkBuffer vertex_buffer = m_vertex_buffer.getBuffer();
	// Vertex input binding is by default at 0u.
	vkCmdBindVertexBuffers(command_buffer, 0u, 1u, &vertex_buffer, &offset);

	// Bind the index buffer
	VkBuffer index_buffer = m_index_buffer.getBuffer();
	vkCmdBindIndexBuffer(command_buffer, index_buffer, 0u, VK_INDEX_TYPE_UINT16);

	// Draw indexed quad (2 triangles = 6 indices)
	vkCmdDrawIndexed(command_buffer, 6u, 1u, 0u, 0, 0u);

	// Our own end of rendering.
	m_vulkan_window.endRendering();

	// Possible to use the command buffer for tasks after rendering to the current frame.

	return true;
}

void Application::terminate()
{
	// Wait, before we destroy anything.
	vkDeviceWaitIdle(m_device);

	// Pipeline related resources.

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

	// Buffer and resource cleanup
	m_descriptor_set0.destroy();
	m_descriptor_set1.destroy();
	m_vertex_buffer.destroy();
	m_index_buffer.destroy();
	m_uniform_view_buffer.destroy();
	m_uniform_model_buffer.destroy();
	m_texture.destroy();
	m_sampler.destroy();
}
