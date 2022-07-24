#version 450
#extension GL_ARB_separate_shader_objects : enable


vec3 colors[3] = vec3[](
vec3(1.0, 0.0, 0.0),
vec3(0.0, 1.0, 0.0),
vec3(0.0, 0.0, 1.0)
);

layout(binding = 0) uniform UniformBufferObject {
    mat4 mvpMat;
    mat4 mMat;
    mat4 nMat;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 0) out vec3 col;

void main() {
    gl_Position = ubo.mvpMat*vec4(inPosition,1.0);
    col = colors[gl_VertexIndex%3];
}