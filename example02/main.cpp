#include <cstdio>

#include "core/core.h"
#include "gpu/gpu.h"
#include "engine/runtime/runtime.h"

#include "Application.h"

int main()
{
	VulkanRuntime vulkan_runtime{};
	vulkan_runtime.setQueueFlags(VK_QUEUE_COMPUTE_BIT);

	if (!vulkan_runtime.prepareInit(false))
	{
		printf("Vulkan runtime prepare init failed\n");

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

	Application application{ vulkan_runtime.getPhysicalDevice(), vulkan_runtime.getDevice(), vulkan_runtime.getQueueFamilyIndex() };
	if (!application.init())
	{
		printf("Could not initialize application.\n");

		application.terminate();

		vulkan_runtime.terminate();

		return -1;
	}
	printf("Application initialized.\n");

	//
	// Main one time loop
	//

	printf("Application executing.\n");

	if (!vulkan_runtime.loop(application))
	{
		printf("Could not execute application.\n");

		application.terminate();

		vulkan_runtime.terminate();

		return -1;
	}
	printf("Application done.\n");

	//
	// Cleanup
	//

	application.terminate();

	vulkan_runtime.terminate();

	printf("Application terminated.\n");

	return 0;
}
