#include "UniformBlock.h"

#include "VulkanSpirvQuery.h"

UniformBlock::UniformBlock(const VulkanSpirvQuery& query, const std::string& block_name)
{
	auto block_info = query.gatherDescriptorSetBufferInfo(block_name);
	if (block_info.has_value())
	{
		m_data.resize(block_info->range);
	}

	auto all_member_names = query.gatherDescriptorSetBlockMemberNames();

	std::string prefix = block_name + ".";

	for (const auto& full_name : all_member_names)
	{
		if (full_name.find(prefix) == 0)
		{
			auto member_info = query.gatherDescriptorSetBufferInfo(full_name);
			if (member_info.has_value())
			{
				UniformMemberInfo info{};
				info.name = full_name.substr(prefix.length());
				info.offset = member_info->offset;
				info.size = member_info->range;

				m_member_infos.push_back(info);
			}
		}
	}
}

VkDeviceSize UniformBlock::size() const
{
	return static_cast<VkDeviceSize>(m_data.size());
}
