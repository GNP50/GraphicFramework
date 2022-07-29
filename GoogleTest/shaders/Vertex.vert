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
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 textCoords;


layout(location = 0) out vec2 col;

void main() {
    gl_Position = ubo.mvpMat*ubo.mMat*vec4(inPosition,1.0);
    col = textCoords;
}