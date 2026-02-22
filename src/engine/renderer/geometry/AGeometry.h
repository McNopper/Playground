#ifndef ENGINE_RENDERER_GEOMETRY_AGEOMETRY_H_
#define ENGINE_RENDERER_GEOMETRY_AGEOMETRY_H_

#include <volk.h>

class AGeometry
{

public:

    virtual ~AGeometry() = default;

    // Bind geometry buffers for rendering
    virtual void bind(VkCommandBuffer command_buffer) const = 0;

    // Issue draw call
    virtual void draw(VkCommandBuffer command_buffer) const = 0;
};

#endif /* ENGINE_RENDERER_GEOMETRY_AGEOMETRY_H_ */
