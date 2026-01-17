#include <gtest/gtest.h>

#include <map>
#include <string>

#include "core/core.h"

#include "gpu/gpu.h"

TEST(TestGlsl, Build)
{
    auto shader_source = load("../resources/shaders/test_glsl.vert");

    EXPECT_TRUE(shader_source.has_value());

    if (shader_source.has_value())
    {
        const std::map<std::string, std::string> macros{};
        auto spirv = buildGlsl(*shader_source, macros, SpvExecutionModelVertex);

        EXPECT_TRUE(spirv.has_value());

        if (spirv.has_value())
        {
            EXPECT_TRUE(!spirv->code.empty());

            EXPECT_EQ(spirv->execution_model, SpvExecutionModelVertex);
        }
    }
}

TEST(TestGlsl, PipelineVertexInputAttributeDescription)
{
    auto shader_source = load("../resources/shaders/test_glsl.vert");

    EXPECT_TRUE(shader_source.has_value());

    if (shader_source.has_value())
    {
        const std::map<std::string, std::string> macros{};
        auto spirv = buildGlsl(*shader_source, macros, SpvExecutionModelVertex);

        EXPECT_TRUE(spirv.has_value());

        if (spirv.has_value())
        {
            EXPECT_TRUE(!spirv->code.empty());

            EXPECT_EQ(spirv->execution_model, SpvExecutionModelVertex);

            std::vector<SpirvData> shaders{ *spirv };
            VulkanSpirvQuery spirv_query{ shaders };

            auto vertex_input_attribute_descriptions = spirv_query.gatherVertexInputAttributeDescriptions(0u);
            
            EXPECT_EQ(vertex_input_attribute_descriptions.size(), 3u);
        }
    }
}
