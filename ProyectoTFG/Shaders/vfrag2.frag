#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    float time;
    vec2 resolution;
    vec3 cameraEye;
    vec3 cameraFront;
    vec3 worldUp;
    mat4 viewMat;
} ubo;

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(vec3(ubo.time/1000.0, ubo.time/1000.0, ubo.time/1000.0), 1.0);
}