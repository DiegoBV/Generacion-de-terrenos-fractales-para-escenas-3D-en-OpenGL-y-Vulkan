#version 450 core
#extension GL_ARB_separate_shader_objects : enable

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

layout(location = 0) out vec4 outColor;

float near = 0.1; 
float far  = 100.0; 

#include ..\\..\\Shaders\\scene0.c

vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord) {
    vec2 uv = vec2(fragCoord.x - (size.x/2.0), fragCoord.y - (size.y/2.0));
    uv.y = ubo.yDirection * uv.y;
    float z = size.y / tan(radians(fieldOfView) / 2.0);
    return normalize(vec3(uv, -z));
}

void main()
{
    vec2 pixelCoord = gl_FragCoord.xy;
    vec3 rayOrigin = ubo.cameraEye;
    vec3 rayDir = rayDirection(45.0, ubo.resolution, pixelCoord);
    rayDir = (ubo.viewMat * vec4(rayDir, 0.0)).xyz; //works

    float distanceToSurface = rayMarch(rayOrigin, rayDir, MIN_DIST, MAX_DIST, ubo.time);

    vec3 p = rayOrigin + rayDir * distanceToSurface;

    float dif = getLight(p, ubo.time);

    gl_FragDepth = distanceToSurface/(far + 1); // divide by far for demonstration
    if (distanceToSurface > MAX_DIST - EPSILON) {
        // Didn't hit anything
        outColor = vec4(0.1, 0.1, 0.1, 1.0);
		return;
    }

    // Use the surface normal as the ambient color of the material
    vec3 K_a = (getNormal(p, ubo.time) + vec3(1.0)) / 2.0;
    vec3 K_d = K_a;
    vec3 K_s = vec3(1.0, 1.0, 1.0);
    float shininess = 10.0;
    
    vec3 phongColor = phongIllumination(K_a, K_d, K_s, shininess, p, ubo.cameraEye, ubo.time);

    vec3 lightColor = vec3(dif);
    outColor = vec4(phongColor * lightColor, 1.0);
}