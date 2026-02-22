#include "VulkanShaderModuleFactory.h"

VulkanShaderModuleFactory::VulkanShaderModuleFactory(VkDevice device, const std::vector<uint32_t>& code) :
    m_device{ device },
    m_code{ code }
{
}

VkShaderModule VulkanShaderModuleFactory::create() const
{
    VkShaderModuleCreateInfo shader_module_create_info{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
    shader_module_create_info.flags = m_flags;
    if (!m_code.empty())
    {
        shader_module_create_info.codeSize = m_code.size() * sizeof(uint32_t);
        shader_module_create_info.pCode = m_code.data();
    }

    VkShaderModule shader_module{ VK_NULL_HANDLE };
    vkCreateShaderModule(m_device, &shader_module_create_info, nullptr, &shader_module);

    return shader_module;
}
