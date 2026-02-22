#ifndef GPU_VULKAN_FACTORY_VULKANINSTANCEFACTORY_H_
#define GPU_VULKAN_FACTORY_VULKANINSTANCEFACTORY_H_

#include <cstdint>
#include <string>
#include <vector>

#include <volk.h>

class VulkanInstanceFactory
{

private:

    std::string m_application_name{ "Example" };
    std::string m_engine_name{ "Playground" };

    uint32_t m_application_version{ VK_MAKE_API_VERSION(0u, 1u, 0u, 0u) };
    uint32_t m_engine_version{ VK_MAKE_API_VERSION(0u, 1u, 0u, 0u) };
    uint32_t m_api_version{ VK_MAKE_API_VERSION(0u, 1u, 4u, 0u) };

    VkInstanceCreateFlags m_instance_create_flags{ 0u };

    std::vector<const char*> m_enabled_layer_names{};
    std::vector<const char*> m_enabled_extension_names{};

public:

    const std::vector<const char*>& getEnabledLayerNames() const;
    bool addEnabledLayerName(const char* name);

    const std::vector<const char*>& getEnabledExtensionNames() const;
    bool addEnabledExtensionName(const char* name);

    VkInstance create() const;
};

#endif /* GPU_VULKAN_FACTORY_VULKANINSTANCEFACTORY_H_ */
