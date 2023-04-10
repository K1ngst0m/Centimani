#version 450

layout (location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inColor;
layout(location = 4) in vec4 inTangent;

struct Camera{
    mat4 view;
    mat4 proj;
    vec3 viewPos;
};
layout (set = 0, binding = 2) uniform CameraUB{
    Camera cameras[100];
};

layout (location = 0) out vec3 outUVW;

out gl_PerVertex
{
    vec4 gl_Position;
};


void main()
{
    outUVW = inPos;
    gl_Position = cameras[0].proj * cameras[0].view * vec4(inPos.xyz, 1.0);
}
