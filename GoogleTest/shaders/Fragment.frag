
#version 450

layout(binding = 1) uniform sampler2D tex1;


layout(location = 0) out vec4 outColor;
layout(location = 0) in vec2 fragPos;

void main() {
    outColor = texture(tex1,fragPos);
}
