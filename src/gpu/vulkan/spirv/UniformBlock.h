#ifndef GPU_VULKAN_SPIRV_UNIFORMBLOCK_H_
#define GPU_VULKAN_SPIRV_UNIFORMBLOCK_H_

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include <volk.h>

class VulkanSpirvQuery;

struct UniformMemberInfo
{
    std::string name{};
    VkDeviceSize offset{ 0u };
    VkDeviceSize size{ 0u };
};

class UniformBlock
{

private:

    std::vector<std::uint8_t> m_data{};
    std::vector<UniformMemberInfo> m_member_infos{};

public:

    UniformBlock() = delete;

    UniformBlock(const VulkanSpirvQuery& query, const std::string& block_name);

    VkDeviceSize size() const;

    const std::vector<std::uint8_t>& getData() const;

    std::vector<std::string> getMemberNames() const;

    template<typename T>
    bool setMember(const std::string& name, const T& value);
};

template<typename T>
bool UniformBlock::setMember(const std::string& name, const T& value)
{
    for (std::size_t i = 0u; i < m_member_infos.size(); i++)
    {
        if (m_member_infos[i].name == name)
        {
            if (sizeof(T) > m_member_infos[i].size)
            {
                return false;
            }

            std::memcpy(m_data.data() + m_member_infos[i].offset, &value, sizeof(T));
            return true;
        }
    }
    return false;
}

#endif /* GPU_VULKAN_SPIRV_UNIFORMBLOCK_H_ */
