#include "Application.h"

#include <map>
#include <string>
#include <vector>

Application::Application(VkPhysicalDevice physical_device, VkDevice device, uint32_t queue_family_index, IVulkanWindow& vulkan_window) :
	m_physical_device{ physical_device }, m_device{ device }, m_queue_family_index{ queue_family_index }, m_vulkan_window{ vulkan_window },
	m_vertex_buffer(physical_device, device),
	m_uniform_view_buffer(physical_device, device),
	m_uniform_model_buffer(physical_device, device),
	m_descriptor_buffer(physical_device, device)
{
}

bool Application::init()
{
	// Only position and color information, having the same structure in the shader.

	struct VertexInputData
	{
		float3	position;
		float4	color;
	};

	// Counter-clockwise triangle using normalized device coordinates having a red, green and blue edge.

	std::vector<VertexInputData> vertex_buffer_data{
		{{ -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }},
		{{ -0.5f, +0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }},
		{{ +0.5f, +0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }}
	};

	// Create vertex buffer using template method
	if (!m_vertex_buffer.create(vertex_buffer_data))
	{
		return false;
	}

	// Create uniform buffers for descriptor buffer usage
	// UniformBuffer automatically includes VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
	if (!m_uniform_view_buffer.create(sizeof(UniformViewData)))
	{
		return false;
	}

	if (!m_uniform_model_buffer.create(sizeof(UniformModelData)))
	{
		return false;
	}

	//
	// Until here, data was created. Now preparing the graphics pipeline.
	//

	// Colored triangle shader.

	std::map<std::string, std::string> macros{};
	auto shaders = buildSlang("colored_triangle.slang", macros, "../resources/shaders/");
	if (shaders.empty())
	{
		return false;
	}

	//
	// Create descriptor buffer with 2 uniform buffer descriptors
	//

	if (!m_descriptor_buffer.create(2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER))
	{
		return false;
	}

	// Write uniform buffer descriptors to the descriptor buffer

	VkDescriptorBufferInfo descriptor_buffer_info_view{};
	descriptor_buffer_info_view.buffer = m_uniform_view_buffer.getBuffer();
	descriptor_buffer_info_view.offset = 0u;
	descriptor_buffer_info_view.range = sizeof(UniformViewData);

	if (!m_descriptor_buffer.writeBufferDescriptor(0, descriptor_buffer_info_view))
	{
		return false;
	}

	VkDescriptorBufferInfo descriptor_buffer_info_model{};
	descriptor_buffer_info_model.buffer = m_uniform_model_buffer.getBuffer();
	descriptor_buffer_info_model.offset = 0u;
	descriptor_buffer_info_model.range = sizeof(UniformModelData);

	if (!m_descriptor_buffer.writeBufferDescriptor(1, descriptor_buffer_info_model))
	{
		return false;
	}

	//
	// Pipeline layout creation with descriptor buffer support
	//

	VulkanPipelineLayoutFactory pipeline_layout_factory{ m_device, 0u };
	
	// Need to create a descriptor set layout for the pipeline layout
	VulkanSpirvQuery spirv_query{ shaders };
	auto descriptor_set_layout_bindings = spirv_query.gatherDescriptorSetLayoutBindings();

	VkDescriptorSetLayoutCreateFlags layout_flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_DESCRIPTOR_BUFFER_BIT_EXT;
	VulkanDescriptorSetLayoutFactory descriptor_set_layout_factory{ m_device, layout_flags };
	for (const auto& descriptor_set_layout_binding : descriptor_set_layout_bindings)
	{
		descriptor_set_layout_factory.addDescriptorSetLayoutBinding(descriptor_set_layout_binding);
	}
	VkDescriptorSetLayout descriptor_set_layout = descriptor_set_layout_factory.create();
	if (descriptor_set_layout == VK_NULL_HANDLE)
	{
		return false;
	}

	pipeline_layout_factory.addDescriptorSetLayout(descriptor_set_layout);
	m_pipeline_layout = pipeline_layout_factory.create();
	
	// Clean up temporary descriptor set layout
	vkDestroyDescriptorSetLayout(m_device, descriptor_set_layout, nullptr);
	
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
	UniformViewData uniform_view_data{};

	// Currently uploading two identity matrices using template method
	if (!m_uniform_view_buffer.update(0u, uniform_view_data))
	{
		return false;
	}

	UniformModelData uniform_model_data{};

	// Currently uploading one identity matrix using template method
	if (!m_uniform_model_buffer.update(0u, uniform_model_data))
	{
		return false;
	}

	// Possible to use the command buffer for tasks before rendering to the current frame.

	// Our own start of rendering.
	m_vulkan_window.beginRendering();

	// Bind and use the graphics pipeline.
	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

	// Bind the descriptor buffer using VK_EXT_descriptor_buffer
	VkDescriptorBufferBindingInfoEXT descriptor_buffer_binding_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT };
	descriptor_buffer_binding_info.address = m_descriptor_buffer.getDeviceAddress();
	descriptor_buffer_binding_info.usage = VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT;

	vkCmdBindDescriptorBuffersEXT(command_buffer, 1, &descriptor_buffer_binding_info);

	// Set descriptor buffer offsets
	uint32_t buffer_index = 0;
	VkDeviceSize descriptor_offset = 0;
	vkCmdSetDescriptorBufferOffsetsEXT(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_layout, 0, 1, &buffer_index, &descriptor_offset);

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

	// ... and draw one triangle.
	vkCmdDraw(command_buffer, 3u, 1u, 0u, 0u);

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

	// Buffer resources are destroyed automatically by RAII destructors
	m_descriptor_buffer.destroy();
	m_vertex_buffer.destroy();
	m_uniform_view_buffer.destroy();
	m_uniform_model_buffer.destroy();
}
