#version 330 core

// RAY MARCHING ALGORITHM
uniform vec2 resolution;
uniform float time;
uniform vec3 cameraEye;
uniform vec3 cameraFront;
uniform vec3 worldUp;
uniform mat4 viewMat;

const int MAX_MARCHING_STEPS = 2550;
const float MIN_DIST = 0.0;
const float MAX_DIST = 1000.0;
const float EPSILON = 0.0001;

// predeclaramos las funciones por si las usan los archivos incluidos
float rayMarch(vec3 eye, vec3 marchingDirection, float start, float end);
vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord);

#include ..\\Shaders\\scene0.c

float rayMarch(vec3 eye, vec3 marchingDirection, float start, float end){
    float depth = start;

    for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
        float dist = sceneSDF(eye + depth * marchingDirection);
        if (dist < EPSILON) {
			return depth;
        }
        depth += dist;
        if (depth >= end) {
            return end;
        }
    }

    return end;
}

vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord) {
    vec2 uv = vec2(fragCoord.x - (size.x/2.0), fragCoord.y - (size.y/2.0));
    float z = size.y / tan(radians(fieldOfView) / 2.0);
    return normalize(vec3(uv, -z));
}

void main()
{
    vec2 pixelCoord = gl_FragCoord.xy;
    vec3 rayOrigin = cameraEye;
    vec3 rayDir = rayDirection(45.0, resolution, pixelCoord);
    rayDir = (viewMat * vec4(rayDir, 0.0)).xyz; //works

    float distanceToSurface = rayMarch(rayOrigin, rayDir, MIN_DIST, MAX_DIST);

    vec3 p = rayOrigin + rayDir * distanceToSurface;

    float dif = getLight(p);

    if (distanceToSurface > MAX_DIST - EPSILON) {
        // Didn't hit anything
        gl_FragColor = vec4(0.1, 0.1, 0.1, 1.0);
		return;
    }

    // Use the surface normal as the ambient color of the material
    vec3 K_a = (getNormal(p) + vec3(1.0)) / 2.0;
    vec3 K_d = K_a;
    vec3 K_s = vec3(1.0, 1.0, 1.0);
    float shininess = 10.0;
    
    vec3 phongColor = phongIllumination(K_a, K_d, K_s, shininess, p, cameraEye);

    vec3 lightColor = vec3(dif);
    gl_FragColor = vec4(phongColor * lightColor, 1.0);
}
