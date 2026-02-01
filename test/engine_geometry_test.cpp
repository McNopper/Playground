#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>

#include "core/core.h"
#include "engine/engine.h"
#include "gpu/gpu.h"

namespace {

struct VulkanHandles {
	VkInstance instance{ VK_NULL_HANDLE };
	VkPhysicalDevice physical_device{ VK_NULL_HANDLE };
	std::uint32_t queue_family_index{ 0u };
	VkDevice device{ VK_NULL_HANDLE };
	VkQueue queue{ VK_NULL_HANDLE };
};

bool initVulkan(VulkanHandles& handles)
{
	VulkanInstanceFactory vulkan_instance_factory{};
	vulkan_instance_factory.addEnabledLayerName("VK_LAYER_KHRONOS_validation");
	handles.instance = vulkan_instance_factory.create();
	if (handles.instance == VK_NULL_HANDLE)
	{
		return false;
	}

	auto physical_devices = gatherPhysicalDevices(handles.instance);
	physical_devices = PhysicalDeviceTypePriorityFilter{ VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU, VK_PHYSICAL_DEVICE_TYPE_CPU } << physical_devices;
	if (physical_devices.empty())
	{
		return false;
	}
	handles.physical_device = physical_devices[0u];

	auto queue_family_properties = gatherPhysicalDeviceQueueFamilyProperties2(handles.physical_device);
	std::vector<std::uint32_t> queue_family_indices(queue_family_properties.size());
	std::generate(queue_family_indices.begin(), queue_family_indices.end(), [index = 0u]() mutable { return index++; });

	queue_family_indices = QueueFamilyIndexFlagsFilter{VK_QUEUE_GRAPHICS_BIT, queue_family_properties} << queue_family_indices;
	if (queue_family_indices.empty())
	{
		return false;
	}
	handles.queue_family_index = queue_family_indices[0u];

	VulkanDeviceFactory vulkan_device_factory{ handles.physical_device, handles.queue_family_index };
	handles.device = vulkan_device_factory.create();
	if (handles.device == VK_NULL_HANDLE)
	{
		return false;
	}

	vkGetDeviceQueue(handles.device, handles.queue_family_index, 0, &handles.queue);

	return true;
}

void terminateVulkan(VulkanHandles& handles)
{
	handles.queue = VK_NULL_HANDLE;

	if (handles.device != VK_NULL_HANDLE)
	{
		vkDestroyDevice(handles.device, nullptr);
		handles.device = VK_NULL_HANDLE;
	}

	handles.queue_family_index = 0u;
	handles.physical_device = VK_NULL_HANDLE;

	if (handles.instance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(handles.instance, nullptr);
		handles.instance = VK_NULL_HANDLE;
	}
}

} // namespace

class TestTriangleMesh : public ::testing::Test
{

protected:

	VulkanSetup m_vulkan_setup{};
	VulkanHandles m_vulkan{};

	void SetUp() override
	{
		ASSERT_TRUE(m_vulkan_setup.init());
		ASSERT_TRUE(initVulkan(m_vulkan));
	}

	void TearDown() override
	{
		terminateVulkan(m_vulkan);
		m_vulkan_setup.terminate();
	}

};

TEST_F(TestTriangleMesh, CreateWithSeparateBuffers)
{
	// Create separate vertex buffers
	std::vector<float3> positions = {
		float3{-1.0f, -1.0f, 0.0f},
		float3{ 1.0f, -1.0f, 0.0f},
		float3{ 0.0f,  1.0f, 0.0f}
	};

	std::vector<float3> normals = {
		float3{0.0f, 0.0f, 1.0f},
		float3{0.0f, 0.0f, 1.0f},
		float3{0.0f, 0.0f, 1.0f}
	};

	std::vector<float2> uvs = {
		float2{0.0f, 0.0f},
		float2{1.0f, 0.0f},
		float2{0.5f, 1.0f}
	};

	VertexBuffer position_buffer{ m_vulkan.physical_device, m_vulkan.device };
	ASSERT_TRUE(position_buffer.create(positions));

	VertexBuffer normal_buffer{ m_vulkan.physical_device, m_vulkan.device };
	ASSERT_TRUE(normal_buffer.create(normals));

	VertexBuffer uv_buffer{ m_vulkan.physical_device, m_vulkan.device };
	ASSERT_TRUE(uv_buffer.create(uvs));

	// Create mesh and set attributes by name
	TriangleMesh mesh{ m_vulkan.physical_device, m_vulkan.device };
	mesh.setVertexAttribute("inPosition", &position_buffer, 0, VK_FORMAT_R32G32B32_SFLOAT);
	mesh.setVertexAttribute("inNormal", &normal_buffer, 1, VK_FORMAT_R32G32B32_SFLOAT);
	mesh.setVertexAttribute("inTexCoord", &uv_buffer, 2, VK_FORMAT_R32G32_SFLOAT);
	mesh.setVertexCount(3);

	// Verify attributes
	EXPECT_TRUE(mesh.hasAttribute("inPosition"));
	EXPECT_TRUE(mesh.hasAttribute("inNormal"));
	EXPECT_TRUE(mesh.hasAttribute("inTexCoord"));
	EXPECT_FALSE(mesh.hasAttribute("nonexistent"));

	// Check attribute data
	const auto* pos_attr = mesh.getAttribute("inPosition");
	ASSERT_NE(pos_attr, nullptr);
	EXPECT_EQ(pos_attr->binding, 0u);
	EXPECT_EQ(pos_attr->format, VK_FORMAT_R32G32B32_SFLOAT);
	EXPECT_EQ(pos_attr->offset, 0u);

	// Verify mesh state
	EXPECT_TRUE(mesh.isValid());
	EXPECT_EQ(mesh.getVertexCount(), 3u);
	EXPECT_EQ(mesh.getIndexBuffer(), nullptr);
}

TEST_F(TestTriangleMesh, CreateWithIndexBuffer)
{
	// Create vertex data
	std::vector<float3> positions = {
		float3{-1.0f, -1.0f, 0.0f},
		float3{ 1.0f, -1.0f, 0.0f},
		float3{ 1.0f,  1.0f, 0.0f},
		float3{-1.0f,  1.0f, 0.0f}
	};

	std::vector<uint32_t> indices = {
		0, 1, 2,
		2, 3, 0
	};

	VertexBuffer position_buffer{ m_vulkan.physical_device, m_vulkan.device };
	ASSERT_TRUE(position_buffer.create(positions));

	IndexBuffer index_buffer{ m_vulkan.physical_device, m_vulkan.device };
	ASSERT_TRUE(index_buffer.create(indices));

	// Create mesh with index buffer
	TriangleMesh mesh{ m_vulkan.physical_device, m_vulkan.device };
	mesh.setVertexAttribute("inPosition", &position_buffer, 0, VK_FORMAT_R32G32B32_SFLOAT);
	mesh.setIndexBuffer(&index_buffer, VK_INDEX_TYPE_UINT32, static_cast<uint32_t>(indices.size()));

	// Verify
	EXPECT_TRUE(mesh.isValid());
	EXPECT_EQ(mesh.getIndexBuffer(), &index_buffer);
	EXPECT_EQ(mesh.getIndexType(), VK_INDEX_TYPE_UINT32);
	EXPECT_EQ(mesh.getIndexCount(), 6u);
}

TEST_F(TestTriangleMesh, CreateWithInterleavedData)
{
	// Interleaved vertex data: position (12 bytes) + uv (8 bytes) = 20 bytes per vertex
	struct Vertex
	{
		float3 position;
		float2 uv;
	};

	std::vector<Vertex> vertices = {
		{float3{-1.0f, -1.0f, 0.0f}, float2{0.0f, 0.0f}},
		{float3{ 1.0f, -1.0f, 0.0f}, float2{1.0f, 0.0f}},
		{float3{ 0.0f,  1.0f, 0.0f}, float2{0.5f, 1.0f}}
	};

	VertexBuffer interleaved_buffer{ m_vulkan.physical_device, m_vulkan.device };
	ASSERT_TRUE(interleaved_buffer.create(vertices));

	// Create mesh with interleaved attributes
	TriangleMesh mesh{ m_vulkan.physical_device, m_vulkan.device };
	mesh.setVertexAttribute("inPosition", &interleaved_buffer, 0, VK_FORMAT_R32G32B32_SFLOAT, 0, 20);
	mesh.setVertexAttribute("inTexCoord", &interleaved_buffer, 0, VK_FORMAT_R32G32_SFLOAT, 12, 20);
	mesh.setVertexCount(3);

	// Verify both attributes point to same buffer
	const auto* pos_attr = mesh.getAttribute("inPosition");
	const auto* uv_attr = mesh.getAttribute("inTexCoord");
	ASSERT_NE(pos_attr, nullptr);
	ASSERT_NE(uv_attr, nullptr);

	EXPECT_EQ(pos_attr->buffer, uv_attr->buffer);
	EXPECT_EQ(pos_attr->binding, 0u);
	EXPECT_EQ(uv_attr->binding, 0u);
	EXPECT_EQ(pos_attr->offset, 0u);
	EXPECT_EQ(uv_attr->offset, 12u);
	EXPECT_EQ(pos_attr->stride, 20u);
	EXPECT_EQ(uv_attr->stride, 20u);

	EXPECT_TRUE(mesh.isValid());
}

TEST_F(TestTriangleMesh, InvalidMesh)
{
	// Mesh without any attributes should be invalid
	TriangleMesh mesh{ m_vulkan.physical_device, m_vulkan.device };
	EXPECT_FALSE(mesh.isValid());

	// Mesh with attribute but no vertex/index count
	VertexBuffer buffer{ m_vulkan.physical_device, m_vulkan.device };
	std::vector<float3> positions = {float3{0.0f, 0.0f, 0.0f}};
	ASSERT_TRUE(buffer.create(positions));

	mesh.setVertexAttribute("inPosition", &buffer, 0, VK_FORMAT_R32G32B32_SFLOAT);
	EXPECT_FALSE(mesh.isValid()); // No vertex count set

	mesh.setVertexCount(1);
	EXPECT_TRUE(mesh.isValid()); // Now valid
}
