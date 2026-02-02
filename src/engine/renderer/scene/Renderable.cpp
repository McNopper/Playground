#include "Renderable.h"

#include "engine/renderer/geometry/AGeometry.h"
#include "engine/renderer/material/MaterialShader.h"
#include "engine/renderer/backend/common/buffer/UniformBuffer.h"
#include "gpu/gpu.h"

Renderable::Renderable(
    VkPhysicalDevice physical_device,
    VkDevice device,
    std::shared_ptr<AGeometry> geometry,
    std::shared_ptr<MaterialShader> material,
    const float4x4& world_matrix
) :
    m_physical_device{ physical_device },
    m_device{ device },
    m_geometry{ std::move(geometry) },
    m_material{ std::move(material) },
    m_world_matrix{ world_matrix }
{
}

Renderable::~Renderable()
{
    if (m_uniform_model_buffer)
    {
        m_uniform_model_buffer->destroy();
    }
}

bool Renderable::init()
{
    // Create uniform buffer for world transform
    m_uniform_model_buffer = std::make_unique<UniformBuffer>(m_physical_device, m_device);
    
    // Create UniformBlock from material's shader
    if (m_material)
    {
        const auto& spirv_shaders = m_material->getSpirvShaders();
        if (!spirv_shaders.empty())
        {
            VulkanSpirvQuery spirv_query{ spirv_shaders };
            m_uniform_model_block = std::make_shared<UniformBlock>(spirv_query, "UniformModelData");
            
            if (!m_uniform_model_buffer->create(m_uniform_model_block->size()))
            {
                return false;
            }
        }
        else
        {
            return false;
        }

        // Bind uniform buffer to material
        if (!m_material->setUniformBuffer("UniformModelData", m_uniform_model_buffer.get()))
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool Renderable::updateUniforms()
{
    if (!m_uniform_model_block || !m_uniform_model_buffer)
    {
        return false;
    }

    if (!m_uniform_model_block->setMember("u_worldMatrix", m_world_matrix))
    {
        return false;
    }

    if (!m_uniform_model_buffer->update(0u, m_uniform_model_block->getData()))
    {
        return false;
    }

    return true;
}

void Renderable::render(VkCommandBuffer command_buffer) const
{
    // Bind material descriptors
    if (m_material)
    {
        m_material->bind(command_buffer);
    }

    // Bind and draw geometry
    if (m_geometry)
    {
        m_geometry->bind(command_buffer);
        m_geometry->draw(command_buffer);
    }
}

const float4x4& Renderable::getWorldMatrix() const
{
    return m_world_matrix;
}

std::shared_ptr<UniformBlock> Renderable::getUniformBlock() const
{
    return m_uniform_model_block;
}

void Renderable::setWorldMatrix(const float4x4& world_matrix)
{
    m_world_matrix = world_matrix;
}
