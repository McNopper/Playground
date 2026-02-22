#include <cstdio>

#include "core/core.h"
#include "gpu/gpu.h"
#include "engine/runtime/runtime.h"

#include "Application.h"

int main()
{
    //
    // Vulkan Runtime
    //

    VulkanRuntime vulkan_runtime{};
    vulkan_runtime.setSamples(VK_SAMPLE_COUNT_4_BIT);

    if (!vulkan_runtime.prepareInitWindow(1024, 768, "Example06"))
    {
        printf("Window preparation failed\n");

        vulkan_runtime.terminate();

        return -1;
    }

    if (!vulkan_runtime.prepareInit())
    {
        printf("Vulkan runtime prepare init failed\n");

        vulkan_runtime.terminate();

        return -1;
    }

    if (!vulkan_runtime.initWindow())
    {
        printf("Window initialization failed\n");

        vulkan_runtime.terminate();

        return -1;
    }

    if (!vulkan_runtime.init())
    {
        printf("Vulkan runtime init failed\n");

        vulkan_runtime.terminate();

        return -1;
    }

    //
    // Application
    //

    Application application{ vulkan_runtime.getPhysicalDevice(), vulkan_runtime.getDevice(), vulkan_runtime.getQueueFamilyIndex(), *vulkan_runtime.getVulkanWindow() };
    if (!application.init())
    {
        printf("Could not initialize application.\n");

        application.terminate();

        vulkan_runtime.terminate();

        return -1;
    }
    printf("Application initialized.\n");

    //
    // Main loop
    //

    printf("Application executing.\n");

    vulkan_runtime.loopWindow(application);

    printf("Application done.\n");

    //
    // Cleanup
    //

    application.terminate();

    vulkan_runtime.terminate();

    printf("Application terminated.\n");

    return 0;
}
