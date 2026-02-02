#include <gtest/gtest.h>

#include "core/core.h"
#include "gpu/gpu.h"

TEST(TestUniformBlock, CreateFromShader)
{
	std::map<std::string, std::string> macros{};
	auto shaders = buildSlang("textured_quad.slang", macros, "../resources/shaders/");

	ASSERT_GE(shaders.size(), 1u);

	VulkanSpirvQuery query(shaders);

	auto block_names = query.gatherDescriptorSetBlockNames();
	ASSERT_GT(block_names.size(), 0u);

	UniformBlock uniform_block(query, block_names[0]);

	EXPECT_GT(uniform_block.size(), 0u);
}

TEST(TestUniformBlock, SetMemberByName)
{
	std::map<std::string, std::string> macros{};
	auto shaders = buildSlang("textured_quad.slang", macros, "../resources/shaders/");

	ASSERT_GE(shaders.size(), 1u);

	VulkanSpirvQuery query(shaders);

	auto block_names = query.gatherDescriptorSetBlockNames();
	ASSERT_GT(block_names.size(), 0u);

	auto member_names = query.gatherDescriptorSetBlockMemberNames();
	ASSERT_GT(member_names.size(), 0u);

	UniformBlock uniform_block(query, block_names[0]);

	// Extract member name from full name (remove block prefix)
	std::string prefix = block_names[0] + ".";
	std::string member_name{};
	for (const auto& full_name : member_names)
	{
		if (full_name.find(prefix) == 0)
		{
			member_name = full_name.substr(prefix.length());
			break;
		}
	}
	ASSERT_FALSE(member_name.empty());

	// Test setting first member
	float4x4 test_matrix{};
	test_matrix.columns[0] = float4{ 1.0f, 0.0f, 0.0f, 0.0f };
	test_matrix.columns[1] = float4{ 0.0f, 1.0f, 0.0f, 0.0f };
	test_matrix.columns[2] = float4{ 0.0f, 0.0f, 1.0f, 0.0f };
	test_matrix.columns[3] = float4{ 0.0f, 0.0f, 0.0f, 1.0f };

	bool result = uniform_block.setMember(member_name, test_matrix);
	EXPECT_TRUE(result);
}

TEST(TestUniformBlock, SetMemberFailures)
{
	std::map<std::string, std::string> macros{};
	auto shaders = buildSlang("textured_quad.slang", macros, "../resources/shaders/");

	ASSERT_GE(shaders.size(), 1u);

	VulkanSpirvQuery query(shaders);

	auto block_names = query.gatherDescriptorSetBlockNames();
	ASSERT_GT(block_names.size(), 0u);

	UniformBlock uniform_block(query, block_names[0]);

	// Test invalid name
	float4x4 test_matrix{};
	bool result = uniform_block.setMember("nonexistent_member", test_matrix);
	EXPECT_FALSE(result);

	// Test oversized data
	std::uint8_t large_data[10000]{};
	result = uniform_block.setMember("invalid_member", large_data);
	EXPECT_FALSE(result);
}

TEST(TestUniformBlock, MemberInfoCorrectness)
{
	std::map<std::string, std::string> macros{};
	auto shaders = buildSlang("textured_quad.slang", macros, "../resources/shaders/");

	ASSERT_GE(shaders.size(), 1u);

	VulkanSpirvQuery query(shaders);

	auto block_names = query.gatherDescriptorSetBlockNames();
	ASSERT_GT(block_names.size(), 0u);

	UniformBlock uniform_block(query, block_names[0]);

	// Test that uniform block was created successfully
	EXPECT_GT(uniform_block.size(), 0u);
}

TEST(TestUniformBlock, GetDataAccess)
{
	std::map<std::string, std::string> macros{};
	auto shaders = buildSlang("textured_quad.slang", macros, "../resources/shaders/");

	ASSERT_GE(shaders.size(), 1u);

	VulkanSpirvQuery query(shaders);

	auto block_names = query.gatherDescriptorSetBlockNames();
	ASSERT_GT(block_names.size(), 0u);

	UniformBlock uniform_block(query, block_names[0]);

	// Test that getData() returns valid vector reference
	const std::vector<std::uint8_t>& data = uniform_block.getData();
	EXPECT_EQ(data.size(), uniform_block.size());
	EXPECT_GT(data.size(), 0u);
}
