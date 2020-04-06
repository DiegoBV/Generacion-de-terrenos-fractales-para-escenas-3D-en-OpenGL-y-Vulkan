#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

layout(binding = 0) uniform UniformBufferObject {
    float time;
    float yDirection;
    float playerRadius;
    vec2 resolution;
    vec3 cameraEye;
    vec3 cameraFront;
    vec3 worldUp;
    vec3 playerPos;
    vec4 playerColor;
    mat4 viewMat;
    mat4 model;
    mat4 projection;
} ubo;

mat4 viewMatrix(vec3 eye, vec3 front, vec3 up) {
    // Based on gluLookAt man page
    vec3 f = front;
    vec3 s = normalize(cross(f, up));
    vec3 u = cross(s, f);
    return mat4(
        vec4(s, 0.0),
        vec4(u, 0.0),
        vec4(-f, 0.0),
        vec4(0.0, 0.0, 0.0, 1)
    );
}

void main()
{
    TexCoords = aTexCoords;    
    gl_Position = ubo.projection * transpose(ubo.viewMat) * ubo.model * vec4(aPos, 1.0);
}