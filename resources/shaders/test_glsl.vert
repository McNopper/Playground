#version 460 core

layout(binding = 0) uniform UniformBufferObject {
    mat4 transform;
} in_ub;

layout (location = 0) in vec4 i_position;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_texcoord_0;

void main()
{
    gl_Position = in_ub.transform * i_position;
}
