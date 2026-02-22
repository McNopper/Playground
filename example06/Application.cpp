#include "Application.h"

#include <vector>

#include "core/core.h"

Application::Application(VkPhysicalDevice physical_device, VkDevice device, uint32_t queue_family_index, IVulkanWindow& vulkan_window) :
    m_physical_device{ physical_device },
    m_device{ device },
    m_queue_family_index{ queue_family_index },
    m_vulkan_window{ vulkan_window }
{
}

bool Application::init()
{
    struct VertexInputData
    {
        float3 position;
        float2 texCoord;
    };

    // Cube vertices (24 vertices for proper texture mapping per face)
    // UV convention: (0,0) = top-left, (1,1) = bottom-right (Vulkan/glTF standard)
    std::vector<VertexInputData> vertex_buffer_data{
        // Front face (z = +0.5)
        { { -0.5f, -0.5f, +0.5f }, { 0.0f, 1.0f } },
        { { +0.5f, -0.5f, +0.5f }, { 1.0f, 1.0f } },
        { { +0.5f, +0.5f, +0.5f }, { 1.0f, 0.0f } },
        { { -0.5f, +0.5f, +0.5f }, { 0.0f, 0.0f } },

        // Back face (z = -0.5)
        { { +0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f } },
        { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f } },
        { { -0.5f, +0.5f, -0.5f }, { 1.0f, 0.0f } },
        { { +0.5f, +0.5f, -0.5f }, { 0.0f, 0.0f } },

        // Left face (x = -0.5)
        { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f } },
        { { -0.5f, -0.5f, +0.5f }, { 1.0f, 1.0f } },
        { { -0.5f, +0.5f, +0.5f }, { 1.0f, 0.0f } },
        { { -0.5f, +0.5f, -0.5f }, { 0.0f, 0.0f } },

        // Right face (x = +0.5)
        { { +0.5f, -0.5f, +0.5f }, { 0.0f, 1.0f } },
        { { +0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f } },
        { { +0.5f, +0.5f, -0.5f }, { 1.0f, 0.0f } },
        { { +0.5f, +0.5f, +0.5f }, { 0.0f, 0.0f } },

        // Top face (y = +0.5)
        { { -0.5f, +0.5f, +0.5f }, { 0.0f, 0.0f } },
        { { +0.5f, +0.5f, +0.5f }, { 1.0f, 0.0f } },
        { { +0.5f, +0.5f, -0.5f }, { 1.0f, 1.0f } },
        { { -0.5f, +0.5f, -0.5f }, { 0.0f, 1.0f } },

        // Bottom face (y = -0.5)
        { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
        { { +0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f } },
        { { +0.5f, -0.5f, +0.5f }, { 1.0f, 1.0f } },
        { { -0.5f, -0.5f, +0.5f }, { 0.0f, 1.0f } }
    };

    // Index buffer for cube (6 faces * 2 triangles * 3 indices = 36 indices)
    std::vector<uint16_t> index_buffer_data{
        0, 1, 2, 0, 2, 3,       // Front
        4, 5, 6, 4, 6, 7,       // Back
        8, 9, 10, 8, 10, 11,    // Left
        12, 13, 14, 12, 14, 15, // Right
        16, 17, 18, 16, 18, 19, // Top
        20, 21, 22, 20, 22, 23  // Bottom
    };

    // Create vertex buffer
    m_vertex_buffer = std::make_shared<VertexBuffer>(m_physical_device, m_device);
    if (!m_vertex_buffer->create(vertex_buffer_data))
    {
        return false;
    }

    // Create index buffer
    m_index_buffer = std::make_shared<IndexBuffer>(m_physical_device, m_device);
    if (!m_index_buffer->create(index_buffer_data))
    {
        return false;
    }

    // Load and upload texture
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

    auto texture = std::make_shared<Texture2D>(m_physical_device, m_device);
    texture->setExtent(image_data->width, image_data->height);
    texture->setFormat(VK_FORMAT_R8G8B8A8_SRGB);
    texture->setUsage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    texture->setMipLevels(1);

    if (!texture->create())
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

    if (!texture->upload(command_pool, queue, image_data.value()))
    {
        vkDestroyCommandPool(m_device, command_pool, nullptr);
        return false;
    }

    vkDestroyCommandPool(m_device, command_pool, nullptr);

    // Store texture and sampler as shared resources
    m_texture = texture;

    // Create sampler with linear filtering
    m_sampler = std::make_shared<Sampler>(m_device);
    m_sampler->setMagFilter(VK_FILTER_LINEAR);
    m_sampler->setMinFilter(VK_FILTER_LINEAR);
    m_sampler->setAddressMode(VK_SAMPLER_ADDRESS_MODE_REPEAT);

    if (!m_sampler->create())
    {
        return false;
    }

    // Create Texture2DSampler pairing
    m_texture_sampler = std::make_shared<Texture2DSampler>(m_texture, m_sampler);

    // Create MaterialShader - handles shader compilation and descriptor management
    auto material = std::make_shared<MaterialShader>(m_physical_device, m_device, "textured_quad.slang", "../resources/shaders/");

    if (!material->build())
    {
        return false;
    }

    // Create UniformBlock from material's shader
    const auto& spirv_shaders = material->getSpirvShaders();
    if (!spirv_shaders.empty())
    {
        VulkanSpirvQuery spirv_query{ spirv_shaders };
        m_uniform_view_block = std::make_shared<UniformBlock>(spirv_query, "UniformViewData");
    }
    else
    {
        return false;
    }

    // Create view uniform buffer (camera matrices - shared across all renderables)
    m_uniform_view_buffer = std::make_shared<UniformBuffer>(m_physical_device, m_device);
    if (!m_uniform_view_buffer->create(m_uniform_view_block->size()))
    {
        return false;
    }

    // Attach view uniform buffer to material (shared camera data)
    material->setUniformBuffer("UniformViewData", m_uniform_view_buffer);
    material->setTexture2DSampler("u_texture", "u_sampler", m_texture_sampler);

    // Create graphics pipeline using material's layout
    VulkanGraphicsPipelineBuilder graphics_pipeline_builder{ m_device, VK_PIPELINE_CREATE_DESCRIPTOR_BUFFER_BIT_EXT, material->getPipelineLayout() };

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

    m_pipeline = graphics_pipeline_builder.create(material->getSpirvShaders());
    if (m_pipeline == VK_NULL_HANDLE)
    {
        return false;
    }

    // Create TriangleMesh geometry - handles vertex/index binding
    auto mesh = std::make_shared<TriangleMesh>(m_physical_device, m_device);

    // Set vertex attributes matching shader input names
    mesh->setVertexAttribute("i_position", m_vertex_buffer, 0, VK_FORMAT_R32G32B32_SFLOAT, 0, sizeof(VertexInputData));
    mesh->setVertexAttribute("i_texcoord", m_vertex_buffer, 0, VK_FORMAT_R32G32_SFLOAT, sizeof(float3), sizeof(VertexInputData));

    // Set index buffer
    mesh->setIndexBuffer(m_index_buffer, VK_INDEX_TYPE_UINT16, 36);

    if (!mesh->isValid())
    {
        return false;
    }

    // Create Renderable - combines geometry, material, and transform
    // Renderable owns and manages its own UniformModelData buffer
    m_renderable = std::make_shared<Renderable>(m_physical_device, m_device, mesh, material, float4x4(1.0f));

    // Initialize renderable (creates uniform buffer and binds to material)
    if (!m_renderable->init())
    {
        return false;
    }

    // Build descriptor buffers from all attached resources
    if (!material->buildDescriptors())
    {
        return false;
    }

    m_vulkan_window.setClearColor(0.25f, 0.25f, 0.25f, 1.0f);

    return true;
}

bool Application::update(double delta_time, VkCommandBuffer command_buffer)
{
    m_rotation_angle += delta_time * 45.0;

    // Update view matrices (camera - shared across all renderables)
    float aspect = (float)m_vulkan_window.getCurrentExtent().width / (float)m_vulkan_window.getCurrentExtent().height;
    float4x4 projection_matrix = perspective(45.0f, aspect, 0.1f, 100.0f);
    float4x4 view_matrix = lookAt(float3{ 0.0f, 1.0f, 3.0f }, float3{ 0.0f, 0.0f, 0.0f }, float3{ 0.0f, 1.0f, 0.0f });

    m_uniform_view_block->setMember("u_projectionMatrix", projection_matrix);
    m_uniform_view_block->setMember("u_viewMatrix", view_matrix);

    if (!m_uniform_view_buffer->update(0u, m_uniform_view_block->getData()))
    {
        return false;
    }

    // Update model matrix (world transform - per renderable)
    quaternion rot_y = rotateRyQuaternion((float)m_rotation_angle);
    m_renderable->setWorldMatrix(rot_y);

    // Note: For shaders with additional uniforms beyond u_worldMatrix,
    // you can access the UniformBlock directly for introspection and updates:
    //   auto uniform_block = m_renderable->getUniformBlock();
    //
    //   // List all available uniform members
    //   auto member_names = uniform_block->getMemberNames();
    //
    //   // Set custom members
    //   uniform_block->setMember("u_customData", custom_value);
    //   uniform_block->setMember("u_lightPosition", light_pos);
    //
    // Then call updateUniforms() to upload all changes to GPU

    if (!m_renderable->updateUniforms())
    {
        return false;
    }

    m_vulkan_window.beginRendering();

    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

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

    // Render the object
    m_renderable->render(command_buffer);

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

    // Clean up scene object and shared resources
    m_renderable.reset();
    m_texture_sampler.reset();

    if (m_sampler)
    {
        m_sampler->destroy();
    }

    if (m_texture)
    {
        m_texture->destroy();
    }

    if (m_uniform_view_buffer)
    {
        m_uniform_view_buffer->destroy();
    }

    if (m_index_buffer)
    {
        m_index_buffer->destroy();
    }

    if (m_vertex_buffer)
    {
        m_vertex_buffer->destroy();
    }
}
