#include "VulkanSetup.h"

bool VulkanSetup::init()
{
    if (m_initialized)
    {
        return false;
    }

    auto result = volkInitialize();
    if (result != VK_SUCCESS)
    {
        return false;
    }

    if (vkEnumerateInstanceVersion == nullptr)
    {
        return false;
    }

    uint32_t version{ 0u };
    result = vkEnumerateInstanceVersion(&version);
    if (result != VK_SUCCESS)
    {
        return false;
    }

    if (VK_API_VERSION_MAJOR(version) < 1u)
    {
        return false;
    }

    if (VK_API_VERSION_MAJOR(version) == 1u)
    {
        if (VK_API_VERSION_MINOR(version) < 4u)
        {
            return false;
        }
    }

    m_initialized = true;

    return true;
}

void VulkanSetup::terminate()
{
    if (m_initialized)
    {
        volkFinalize();

        m_initialized = false;
    }
}
