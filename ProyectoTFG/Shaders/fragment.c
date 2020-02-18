#version 330 core

// RAY MARCHING ALGORITHM
uniform vec2 resolution;
uniform float time;
uniform vec3 cameraEye;
uniform vec3 cameraFront;
uniform vec3 worldUp;
uniform mat4 viewMat;

vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord);

#include ..\\..\\Shaders\\mandelBulb.c

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
    
    //tracing the ray (getting the distance of the closest object in the ray direction)
	
    float distanceToSurface = rayMarch(rayOrigin, rayDir, MIN_DIST, MAX_DIST);

    vec3 p = rayOrigin + rayDir * distanceToSurface;

    if (distanceToSurface > MAX_DIST - EPSILON) {
        // Didn't hit anything
        gl_FragColor = vec4(0.1, 0.1, 0.1, 1.0);
		return;
    }

    gl_FragColor = vec4(getColor(p), 1.0);  
}
