#include "Renderable.h"
#include "UniformData.h"

#include "engine/renderer/geometry/AGeometry.h"
#include "engine/renderer/material/MaterialShader.h"
#include "engine/renderer/backend/common/buffer/UniformBuffer.h"

Renderable::Renderable(
    VkPhysicalDevice physical_device,
    VkDevice device,
    std::shared_ptr<AGeometry> geometry,
    std::shared_ptr<MaterialShader> material,
    const float4x4& transform
) :
    m_physical_device{ physical_device },
    m_device{ device },
    m_geometry{ std::move(geometry) },
    m_material{ std::move(material) },
    m_transform{ transform }
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
    
    if (!m_uniform_model_buffer->create(sizeof(UniformModelData)))
    {
        return false;
    }

    // Bind uniform buffer to material
    if (m_material)
    {
        if (!m_material->setUniformBuffer("UniformModelData", m_uniform_model_buffer.get()))
        {
            return false;
        }
    }

    return true;
}

const std::shared_ptr<AGeometry>& Renderable::getGeometry() const
{
    return m_geometry;
}

const std::shared_ptr<MaterialShader>& Renderable::getMaterial() const
{
    return m_material;
}

UniformBuffer* Renderable::getUniformModelBuffer() const
{
    return m_uniform_model_buffer.get();
}

const float4x4& Renderable::getTransform() const
{
    return m_transform;
}

void Renderable::setGeometry(std::shared_ptr<AGeometry> geometry)
{
    m_geometry = std::move(geometry);
}

void Renderable::setMaterial(std::shared_ptr<MaterialShader> material)
{
    m_material = std::move(material);
}

void Renderable::setTransform(const float4x4& transform)
{
    m_transform = transform;
}
