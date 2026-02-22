#include <iostream>
#include <map>
#include <string>

#include <gtest/gtest.h>

#include "gpu/gpu.h"
#include "gpu/vulkan/spirv/VulkanSpirvQuery.h"

TEST(TestSpirv, Load)
{
    auto spirv = loadSpirv("../resources/shaders/compute_grey.spirv");

    EXPECT_TRUE(spirv.has_value());

    if (spirv.has_value())
    {
        EXPECT_TRUE(!spirv->code.empty());

        EXPECT_EQ(spirv->execution_model, SpvExecutionModelGLCompute);
    }
}

TEST(TestSpirv, ReflectTexturedQuad)
{
    std::map<std::string, std::string> macros{};
    auto shaders = buildSlang("textured_quad.slang", macros, "../resources/shaders/");

    ASSERT_GE(shaders.size(), 1u);

    VulkanSpirvQuery query(shaders);

    // Print Descriptor Set Layout Bindings
    auto descriptor_bindings = query.gatherDescriptorSetLayoutBindings();
    std::cout << "\n=== Descriptor Set Layout Bindings ===\n";
    std::cout << "Count: " << descriptor_bindings.size() << '\n';
    for (const auto& binding : descriptor_bindings)
    {
        std::cout << "  Binding " << binding.binding << ":\n";
        std::cout << "    Descriptor Type: " << binding.descriptorType << '\n';
        std::cout << "    Descriptor Count: " << binding.descriptorCount << '\n';
        std::cout << "    Stage Flags: 0x" << std::hex << binding.stageFlags << std::dec << '\n';
    }

    // Print Descriptor Set Block Names (Uniform Buffers only)
    auto block_names = query.gatherDescriptorSetBlockNames();
    std::cout << "\n=== Descriptor Set Block Names ===\n";
    std::cout << "Count: " << block_names.size() << '\n';
    for (const auto& name : block_names)
    {
        std::cout << "  " << name << '\n';

        // Print buffer info for block
        auto buffer_info = query.gatherDescriptorSetBufferInfo(name);
        if (buffer_info.has_value())
        {
            std::cout << "    Offset: " << buffer_info->offset << '\n';
            std::cout << "    Range: " << buffer_info->range << '\n';
        }
    }

    // Print Descriptor Set Block Member Names
    auto member_names = query.gatherDescriptorSetBlockMemberNames();
    std::cout << "\n=== Descriptor Set Block Member Names ===\n";
    std::cout << "Count: " << member_names.size() << '\n';
    for (const auto& name : member_names)
    {
        std::cout << "  " << name << '\n';

        // Print buffer info for each member
        auto buffer_info = query.gatherDescriptorSetBufferInfo(name);
        if (buffer_info.has_value())
        {
            std::cout << "    Offset: " << buffer_info->offset << '\n';
            std::cout << "    Range: " << buffer_info->range << '\n';
        }
    }

    // Print Push Constant Ranges
    auto push_constant_ranges = query.gatherPushConstantRanges();
    std::cout << "\n=== Push Constant Ranges ===\n";
    std::cout << "Count: " << push_constant_ranges.size() << '\n';
    for (const auto& range : push_constant_ranges)
    {
        std::cout << "  Stage Flags: 0x" << std::hex << range.stageFlags << std::dec << '\n';
        std::cout << "  Offset: " << range.offset << '\n';
        std::cout << "  Size: " << range.size << '\n';
    }

    // Print Vertex Input Names
    auto vertex_input_names = query.gatherVertexInputNames();
    std::cout << "\n=== Vertex Input Names ===\n";
    std::cout << "Count: " << vertex_input_names.size() << '\n';
    for (const auto& name : vertex_input_names)
    {
        std::cout << "  " << name << '\n';
    }

    // Print Vertex Input Binding Description
    auto vertex_binding = query.gatherVertexInputBindingDescription(0);
    std::cout << "\n=== Vertex Input Binding Description ===\n";
    if (vertex_binding.has_value())
    {
        std::cout << "Binding: " << vertex_binding->binding << '\n';
        std::cout << "Stride: " << vertex_binding->stride << '\n';
        std::cout << "Input Rate: " << vertex_binding->inputRate << '\n';
    }
    else
    {
        std::cout << "No vertex binding found\n";
    }

    // Print Vertex Input Attribute Descriptions
    auto vertex_attributes = query.gatherVertexInputAttributeDescriptions(0);
    std::cout << "\n=== Vertex Input Attribute Descriptions ===\n";
    std::cout << "Count: " << vertex_attributes.size() << '\n';
    for (const auto& attr : vertex_attributes)
    {
        std::cout << "  Location " << attr.location << ":\n";
        std::cout << "    Binding: " << attr.binding << '\n';
        std::cout << "    Format: " << attr.format << '\n';
        std::cout << "    Offset: " << attr.offset << '\n';
    }

    std::cout << '\n';

    EXPECT_GT(descriptor_bindings.size(), 0u);
    EXPECT_GT(vertex_input_names.size(), 0u);
}
