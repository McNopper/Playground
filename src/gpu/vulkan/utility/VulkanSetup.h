#ifndef GPU_VULKAN_UTILITY_VULKANSETUP_H_
#define GPU_VULKAN_UTILITY_VULKANSETUP_H_

#include <volk.h>

class VulkanSetup
{

private:

    bool m_initialized{ false };

public:

    VulkanSetup() = default;

    VulkanSetup(const VulkanSetup&) = delete;
    VulkanSetup(VulkanSetup&&) = delete;

    VulkanSetup operator=(const VulkanSetup&) = delete;
    VulkanSetup operator=(VulkanSetup&&) = delete;

    bool init();

    void terminate();

};

#endif /* GPU_VULKAN_UTILITY_VULKANSETUP_H_ */
