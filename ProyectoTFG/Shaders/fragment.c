#version 330 core

#include ..\\Shaders\\geometries.c

uniform vec2 resolution;
uniform float time;
uniform vec3 cameraEye;
uniform vec3 cameraFront;
uniform vec3 worldUp;

const int MAX_MARCHING_STEPS = 2550;
const float MIN_DIST = 0.0;
const float MAX_DIST = 1000.0;
const float EPSILON = 0.0001;

float intersectSDF(float distA, float distB) {
    return max(distA, distB);
}

float unionSDF(float distA, float distB) {
    return min(distA, distB);
}

float differenceSDF(float distA, float distB) {
    return max(distA, -distB);
}

float SDFCylinder( vec3 p, float h, float r )
{
  vec2 d = abs(vec2(length(p.xy),p.z)) - vec2(r,h);
  return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

float getDistance(vec3 point){
    Sphere sphere;
    sphere.center = vec3(1.0, 0.0, 0.0);
    sphere.radius = 3.0;
    sphere.color = vec4(1.0, 0.0, 0.0, 1.0);
    Box box;
    box.center = vec3(0.0, 0.0, 0.0);
    box.dimensions = vec3(2.5, 2.5, 2.5);
    box.color = vec4(0.0, 1.0, 0.0, 1.0);
    box.rotation = mat3(
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
        );
    box.rotateZ(30.0);
    
    float radiusCylinder = 1.5;
    float height = 3.0;

    float intersection = intersectSDF(box.SDF(point), sphere.SDF(point));
    intersection = differenceSDF(intersection, SDFCylinder(point, height, radiusCylinder));
    return min(intersection, point.y + 5.0);
}

float rayMarch(vec3 eye, vec3 marchingDirection, float start, float end){
    float depth = start;

    for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
        float dist = getDistance(eye + depth * marchingDirection);
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

vec3 getNormal(vec3 p){
    return normalize(vec3(
        getDistance(vec3(p.x + EPSILON, p.y, p.z)) - getDistance(vec3(p.x - EPSILON, p.y, p.z)),
        getDistance(vec3(p.x, p.y + EPSILON, p.z)) - getDistance(vec3(p.x, p.y - EPSILON, p.z)),
        getDistance(vec3(p.x, p.y, p.z  + EPSILON)) - getDistance(vec3(p.x, p.y, p.z - EPSILON))
    ));
}

float getLight(vec3 p){
    vec3 lightPos = vec3(0, 5, 0);
    lightPos.x += cos(time)*5.0;
    lightPos.z += sin(time)*5.0;
    vec3 light = normalize(lightPos - p);
    vec3 normal = getNormal(p);

    float dif = dot(normal, light);
    float distancia = rayMarch(p + (normal * EPSILON * 2.0), light, MIN_DIST, MAX_DIST);
    if(distancia < length(lightPos - p)) dif *= 0.1;
    return dif;
}

mat4 viewMatrix(vec3 eye, vec3 center, vec3 up) {
    // Based on gluLookAt man page
    vec3 f = normalize(center -eye);
    vec3 s = normalize(cross(f, up));
    vec3 u = cross(s, f);
    return mat4(
        vec4(s, 0.0),
        vec4(u, 0.0),
        vec4(-f, 0.0),
        vec4(0.0, 0.0, 0.0, 1)
    );
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
    mat4 viewToWorld = viewMatrix(rayOrigin, cameraEye + cameraFront, worldUp);
    rayDir = (viewToWorld * vec4(rayDir, 0.0)).xyz; //works

    float distanceToSurface = rayMarch(rayOrigin, rayDir, MIN_DIST, MAX_DIST);

    vec3 p = rayOrigin + rayDir * distanceToSurface;

    float dif = getLight(p);

    if (distanceToSurface > MAX_DIST - EPSILON) {
        // Didn't hit anything
        gl_FragColor = vec4(0.1, 0.1, 0.1, 1.0);
		return;
    }

    vec3 col = vec3(dif);
    gl_FragColor = vec4(col, 1.0);
}
