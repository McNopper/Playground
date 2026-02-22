#ifndef ENGINE_RUNTIME_IAPPLICATION_H_
#define ENGINE_RUNTIME_IAPPLICATION_H_

#include <volk.h>

class IVulkanWindow;

/**
 * IApplication - Interface for VulkanRuntime applications
 * 
 * Applications must implement:
 *   - init(): Initialize resources (called once at startup)
 *   - update(): Update and render (called every frame/iteration)
 *   - terminate(): Cleanup resources (called once at shutdown)
 */
class IApplication
{

public:

    virtual ~IApplication() = default;

    virtual bool init() = 0;

    virtual bool update(double delta_time, VkCommandBuffer command_buffer) = 0;

    virtual void terminate() = 0;

};

#endif /* ENGINE_RUNTIME_IAPPLICATION_H_ */
