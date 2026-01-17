#ifndef GPU_VULKAN_FACTORY_VULKANSHADERMODULEFACTORY_H_
#define GPU_VULKAN_FACTORY_VULKANSHADERMODULEFACTORY_H_

#include <cstdint>
#include <vector>

#include <volk.h>

class VulkanShaderModuleFactory
{

private:

    VkDevice m_device{ VK_NULL_HANDLE };

    VkShaderModuleCreateFlags m_flags{};
    std::vector<uint32_t> m_code{};

public:

    VulkanShaderModuleFactory() = delete;

    VulkanShaderModuleFactory(VkDevice device, const std::vector<uint32_t>& code);

    VkShaderModule create() const;

};

#endif /* GPU_VULKAN_FACTORY_VULKANSHADERMODULEFACTORY_H_ */
