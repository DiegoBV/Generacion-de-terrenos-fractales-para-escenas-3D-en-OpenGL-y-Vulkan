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

vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord);

float near = 0.1; 
float far  = 100.0; 

#include ..\\..\\Shaders\\geometries.c
#include ..\\..\\Shaders\\oceanTerrain.c

vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord) {
    vec2 uv = vec2(fragCoord.x - (size.x/2.0), fragCoord.y - (size.y/2.0));
    uv.y = ubo.yDirection * uv.y;
    float nearPlane = size.y / tan(radians(fieldOfView) / 2.0);
    return normalize(vec3(uv, -nearPlane));
}

float rayMarchDebug(vec3 eye, vec3 marchingDirection, float start, float end, Sphere sphere){
    float depth = start;

    for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
        float dist = sphereSDF(sphere, eye + depth * marchingDirection);
        if (dist < EPSILON) {
			break;
        }
        depth += 0.5 * dist;
        if (depth >= end) {
            break;
        }
    }

    return depth;
}

void main()
{
    vec2 pixelCoord = gl_FragCoord.xy;
    vec3 rayOrigin = ubo.cameraEye;
    vec3 rayDir = rayDirection(45.0, ubo.resolution, pixelCoord);
    rayDir = (ubo.viewMat * vec4(rayDir, 0.0)).xyz; //works

    mat3 identity = mat3(1, 0, 0, 0, 1, 0, 0, 0, 1);
    Sphere sphere; sphere.center = ubo.playerPos; sphere.radius = ubo.playerRadius; sphere.color = ubo.playerColor;
    sphere.rotation = identity;
    
    //tracing the ray (getting the distance of the closest object in the ray direction)
	
    float fractal = rayMarch(rayOrigin, rayDir, MIN_DIST, MAX_DIST, ubo.time);
    float pelota = rayMarchDebug(rayOrigin, rayDir, MIN_DIST, MAX_DIST, sphere);

    float distanceToSurface = 0.0f;
    if(ubo.debug){
        distanceToSurface = min(fractal, pelota);
    }
    else {
        distanceToSurface = fractal;
    }
    
    //gl_FragDepth = distanceToSurface/(far + 1); // divide by far for demonstration
    
    //DEBUG (modelo)
    if(ubo.debug && pelota < fractal){
        outColor = sphere.color;
		return;
    }

    vec3 p = rayOrigin + rayDir * distanceToSurface;

    outColor = vec4(getColor(p, rayOrigin, rayDir, ubo.resolution, gl_FragCoord.xy, ubo.viewMat, ubo.yDirection, ubo.time), 1);
}
