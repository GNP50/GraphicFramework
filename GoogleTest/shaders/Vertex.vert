#version 450




layout(location = 0) out vec3 fragColor;



void main() {
    printf("Ciao");
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = colors[gl_VertexIndex];
}
