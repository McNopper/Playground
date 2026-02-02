#version 460 core

layout(binding = 0) uniform UniformBufferObject {
    mat4 u_transform;
} uniform_buffer;

layout (location = 0) in vec4 i_position;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_texcoord;

void main()
{
    gl_Position = uniform_buffer.u_transform * i_position;
}
