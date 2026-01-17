#include <gtest/gtest.h>

#include <map>
#include <string>

#include "gpu/gpu.h"

TEST(TestSlang, Build)
{
    std::map<std::string, std::string> macros{};
    auto shaders = buildSlang("compute_grey.slang", macros, "../resources/shaders/");

    EXPECT_EQ(shaders.size(), 1u);
}
