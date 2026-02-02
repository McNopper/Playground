#include "TriangleMesh.h"

#include "engine/renderer/backend/common/buffer/VertexBuffer.h"
#include "engine/renderer/backend/common/buffer/IndexBuffer.h"

TriangleMesh::TriangleMesh(VkPhysicalDevice physical_device, VkDevice device) :
    m_physical_device{ physical_device }, m_device{ device }
{
}

TriangleMesh::~TriangleMesh()
{
    // Note: We don't own the buffers, so we don't delete them
}

void TriangleMesh::setVertexAttribute(
    const std::string& name,
    const std::shared_ptr<VertexBuffer>& buffer,
    uint32_t binding,
    VkFormat format,
    uint32_t offset,
    uint32_t stride)
{
    VertexAttribute attr{};
    attr.buffer = buffer;
    attr.binding = binding;
    attr.offset = offset;
    attr.stride = stride;
    attr.format = format;

    m_vertex_attributes[name] = attr;
}

void TriangleMesh::setIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer, VkIndexType index_type, uint32_t count)
{
    m_index_buffer = buffer;
    m_index_type = index_type;
    m_index_count = count;
}

void TriangleMesh::setVertexCount(uint32_t count)
{
    m_vertex_count = count;
}

bool TriangleMesh::hasAttribute(const std::string& name) const
{
    return m_vertex_attributes.contains(name);
}

const TriangleMesh::VertexAttribute* TriangleMesh::getAttribute(const std::string& name) const
{
    auto it = m_vertex_attributes.find(name);
    if (it != m_vertex_attributes.end())
    {
        return &it->second;
    }
    return nullptr;
}

std::shared_ptr<IndexBuffer> TriangleMesh::getIndexBuffer() const
{
    return m_index_buffer;
}

VkIndexType TriangleMesh::getIndexType() const
{
    return m_index_type;
}

uint32_t TriangleMesh::getIndexCount() const
{
    return m_index_count;
}

uint32_t TriangleMesh::getVertexCount() const
{
    return m_vertex_count;
}

void TriangleMesh::bind(VkCommandBuffer command_buffer) const
{
    if (!isValid())
    {
        return;
    }

    // Collect unique bindings and their buffers
    std::map<uint32_t, VkBuffer> binding_to_buffer{};
    std::map<uint32_t, VkDeviceSize> binding_to_offset{};

    for (const auto& [name, attr] : m_vertex_attributes)
    {
        if (attr.buffer && attr.buffer->isValid())
        {
            binding_to_buffer[attr.binding] = attr.buffer->getBuffer();
            binding_to_offset[attr.binding] = 0u; // Offset handled per-attribute
        }
    }

    // Bind vertex buffers
    if (!binding_to_buffer.empty())
    {
        std::vector<VkBuffer> buffers{};
        std::vector<VkDeviceSize> offsets{};

        for (const auto& [binding, buffer] : binding_to_buffer)
        {
            buffers.push_back(buffer);
            offsets.push_back(binding_to_offset[binding]);
        }

        vkCmdBindVertexBuffers(
            command_buffer,
            0u, // First binding
            static_cast<uint32_t>(buffers.size()),
            buffers.data(),
            offsets.data()
        );
    }

    // Bind index buffer if present
    if (m_index_buffer && m_index_buffer->isValid())
    {
        vkCmdBindIndexBuffer(
            command_buffer,
            m_index_buffer->getBuffer(),
            0u,
            m_index_type
        );
    }
}

void TriangleMesh::draw(VkCommandBuffer command_buffer) const
{
    if (!isValid())
    {
        return;
    }

    if (m_index_buffer && m_index_buffer->isValid())
    {
        // Indexed draw
        vkCmdDrawIndexed(command_buffer, m_index_count, 1u, 0u, 0, 0u);
    }
    else
    {
        // Non-indexed draw
        vkCmdDraw(command_buffer, m_vertex_count, 1u, 0u, 0u);
    }
}

bool TriangleMesh::isValid() const
{
    // Must have at least one vertex attribute with valid buffer
    bool has_valid_attribute = false;
    for (const auto& [name, attr] : m_vertex_attributes)
    {
        if (attr.buffer && attr.buffer->isValid())
        {
            has_valid_attribute = true;
            break;
        }
    }

    if (!has_valid_attribute)
    {
        return false;
    }

    // If index buffer is set, it must be valid
    if (m_index_buffer && !m_index_buffer->isValid())
    {
        return false;
    }

    // Must have either vertex count or index count
    if (m_vertex_count == 0u && m_index_count == 0u)
    {
        return false;
    }

    return true;
}

