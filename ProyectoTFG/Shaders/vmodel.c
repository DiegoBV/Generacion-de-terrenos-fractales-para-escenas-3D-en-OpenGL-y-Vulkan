#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

layout(location = 0) out vec2 TexCoords;

layout(binding = 0) uniform UniformBufferObject {
    bool debug;
    float time;
    float yDirection;
    float playerRadius;
    vec2 resolution;
    vec3 fractalRotation;
    vec3 cameraEye;
    vec3 cameraFront;
    vec3 worldUp;
    vec3 playerPos;
    vec4 playerColor;
    mat4 viewMat;
    mat4 model;
    mat4 projection;
} ubo;

void main()
{
    TexCoords = aTexCoords;    
    gl_Position = ubo.projection * transpose(ubo.viewMat) * ubo.model * vec4(aPos, 1.0);
}