#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>

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

}

TEST(TestMaterialShader, BuildTexturedQuad)
{
    VulkanSetup vulkan_setup{};
    ASSERT_TRUE(vulkan_setup.init());

    VulkanHandles handles{};
    ASSERT_TRUE(initVulkan(handles));

    {
        // Create MaterialShader with textured_quad.slang (used in example05)
        MaterialShader material{ handles.physical_device, handles.device, "textured_quad.slang", "../resources/shaders/" };

        // Build the material - should compile shader and reflect pipeline layout
        bool build_result = material.build();
        ASSERT_TRUE(build_result);

        // Verify SPIR-V shaders were generated
        const auto& spirv_shaders = material.getSpirvShaders();
        ASSERT_GE(spirv_shaders.size(), 1u);
        std::cout << "\n=== MaterialShader SPIR-V Shaders ===\n";
        std::cout << "Shader count: " << spirv_shaders.size() << '\n';
        for (const auto& shader : spirv_shaders)
        {
            EXPECT_FALSE(shader.code.empty());
            std::cout << "  Execution model: " << shader.execution_model << '\n';
            std::cout << "  Code size: " << shader.code.size() << " words\n";
        }// Verify pipeline layout was created via reflection
        VkPipelineLayout pipeline_layout = material.getPipelineLayout();
        ASSERT_NE(pipeline_layout, VK_NULL_HANDLE);
        std::cout << "\n=== MaterialShader Pipeline Layout ===\n";
        std::cout << "Pipeline layout: " << pipeline_layout << '\n';

        // Test reflection - verify descriptor bindings were found
        VulkanSpirvQuery query{ spirv_shaders };
        auto descriptor_bindings = query.gatherDescriptorSetLayoutBindings();
        EXPECT_GE(descriptor_bindings.size(), 1u);
        std::cout << "\n=== Reflected Descriptor Bindings ===\n";
        std::cout << "Binding count: " << descriptor_bindings.size() << '\n';
        for (const auto& binding : descriptor_bindings)
        {
            std::cout << "  Binding " << binding.binding << ":\n";
            std::cout << "    Descriptor Type: " << binding.descriptorType << '\n';
            std::cout << "    Descriptor Count: " << binding.descriptorCount << '\n';
            std::cout << "    Stage Flags: 0x" << std::hex << binding.stageFlags << std::dec << '\n';
        }

        // Test vertex input reflection
        auto vertex_binding = query.gatherVertexInputBindingDescription(0);
        EXPECT_TRUE(vertex_binding.has_value());
        if (vertex_binding.has_value())
        {
            std::cout << "\n=== Reflected Vertex Input ===\n";
            std::cout << "Binding: " << vertex_binding->binding << '\n';
            std::cout << "Stride: " << vertex_binding->stride << '\n';
            std::cout << "Input Rate: " << vertex_binding->inputRate << '\n';
        }

        auto vertex_attributes = query.gatherVertexInputAttributeDescriptions(0);
        EXPECT_GE(vertex_attributes.size(), 1u);
        std::cout << "Vertex attribute count: " << vertex_attributes.size() << '\n';
        for (const auto& attr : vertex_attributes)
        {
            std::cout << "  Location " << attr.location << ": format=" << attr.format << ", offset=" << attr.offset << '\n';
        }

        // MaterialShader destructor will cleanup shader modules and pipeline layout here
    }

    // Cleanup
    terminateVulkan(handles);
    vulkan_setup.terminate();
}

TEST(TestMaterialShader, BuildFailsWithInvalidShader)
{
    VulkanSetup vulkan_setup{};
    ASSERT_TRUE(vulkan_setup.init());

    VulkanHandles handles{};
    ASSERT_TRUE(initVulkan(handles));

    {
        // Try to build with non-existent shader
        MaterialShader material{ handles.physical_device, handles.device, "nonexistent_shader.slang", "../resources/shaders/" };

        bool build_result = material.build();
        EXPECT_FALSE(build_result);

        // Verify nothing was created
        const auto& spirv_shaders = material.getSpirvShaders();
        EXPECT_TRUE(spirv_shaders.empty());

        VkPipelineLayout pipeline_layout = material.getPipelineLayout();
        EXPECT_EQ(pipeline_layout, VK_NULL_HANDLE);

        // MaterialShader destructor cleanup
    }

    // Cleanup
    terminateVulkan(handles);
    vulkan_setup.terminate();
}

