#ifndef GPU_VULKAN_FACTORY_VULKANDEVICEFACTORY_H_
#define GPU_VULKAN_FACTORY_VULKANDEVICEFACTORY_H_

#include <vector>

#include <volk.h>

class VulkanDeviceFactory
{

private:

    VkPhysicalDevice m_physical_device{ VK_NULL_HANDLE };

    uint32_t m_queue_family_index{ 0u };

    VkDeviceQueueCreateFlags m_device_queue_create_flags{ 0u };
    std::vector<float> m_queue_priorities{ 1.0f };

    VkDeviceCreateFlags m_device_create_flags{ 0u };
    std::vector<const char*> m_enabled_extension_names{};

public:

    VulkanDeviceFactory() = delete;

    VulkanDeviceFactory(VkPhysicalDevice physical_device, uint32_t queue_family_index);

    const std::vector<const char*>& getEnabledExtensionNames() const;
    bool addEnabledExtensionName(const char* name);

    VkDevice create() const;

};

#endif /* GPU_VULKAN_FACTORY_VULKANDEVICEFACTORY_H_ */
