layout(binding = 0) uniform UniformBufferObject {
    mat4 mvpMat;
    mat4 mMat;
    mat4 nMat;
} ubo;

mat4 rotate(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(
    oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
    oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
    oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
    0.0,                                0.0,                                0.0,                                1.0
    );
}

layout(binding = 1) uniform CameraPositionObject{
    vec4 cameraPos;
    vec4 cameraDir;
    float w;
    float h;
    float n;
    float f;
}camUbo;

layout(binding = 2) uniform TextureMappingObject{
    vec2 rs_u;
    vec2 rs_b;

    vec2 cs_u;
    vec2 cs_b;

    vec2 ls_u;
    vec2 ls_b;
}textureMappingUbo;

vec2 normalazePoint(vec2 toNormalize){
    if(toNormalize.x > 1)
        toNormalize.x -=1;
    if(toNormalize.x < 0)
        toNormalize.x +=1;

    if(toNormalize.y > 1)
        toNormalize.y -=1;
    if(toNormalize.y < 0)
        toNormalize.y +=1;

    return toNormalize;
}



layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 textCoords;


layout(location = 0) out vec2 col;
layout(location = 1) out vec3 _norm;
layout(location = 2) out vec3 _textCoords;

void main() {
    //get the angles
    float theta = acos(dot(vec3(1,0,0),vec3(camUbo.cameraPos.xz,0.0)));
    float alpha = acos(dot(vec3(1,0,0),vec3(camUbo.cameraPos.x,0.0,-camUbo.cameraPos.y)));

    mat4 rotationMatrix = rotate(vec3(0,0,1),theta)*rotate(vec3(1,0,0),theta);

    vec4 rotatedPlane =  rotationMatrix*vec4(inPosition,1.0);
    gl_Position = ubo.mvpMat*ubo.mMat*rotatedPlane;

    _norm = (rotationMatrix*vec4(1,0,0,0)).xyz;
    col = textCoords;
}