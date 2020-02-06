#version 330 core

#include ..\\Shaders\\geometries.c

uniform vec2 resolution;
uniform float time;
uniform vec3 cameraPosition;
uniform vec3 cameraDirection;

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

    return min(sphere.SDF(point), point.y + 5.0);
}

float sceneSDF(vec3 point){
    mat3 identity = mat3(1, 0, 0, 0, 1, 0, 0, 0, 1);
    point = rotateY(degrees(time / 2.0), identity) * point;

    Plane plane; plane.height = 5.0;

    Sphere sphere; sphere.center = vec3(0.0, 0.0, 0.0); sphere.radius = 2.4; sphere.color = vec4(1.0, 0.0, 0.0, 1.0);
    sphere.rotation = identity;
    Box box; box.center = vec3(0.0, 0.0, 0.0); box.dimensions = vec3(2, 2, 2); box.color = vec4(0.0, 1.0, 0.0, 1.0);
    box.rotation = identity;

    float cylinderRadius = 0.8 + (1.0 - 0.4) * (3.0 + sin(1.7 * time)) / 2.0;
    Cylinder cylinder1; cylinder1.center = vec3(0.0, 0.0, 0.0); cylinder1.height = 2.4; cylinder1.radius = cylinderRadius;
    cylinder1.rotation = identity;
    Cylinder cylinder2; cylinder2.center = vec3(0.0, 0.0, 0.0); cylinder2.height = 2.4; cylinder2.radius = cylinderRadius;
    cylinder2.rotation = identity;
    cylinder2.rotateFigX(90.0);
    Cylinder cylinder3; cylinder3.center = vec3(0.0, 0.0, 0.0); cylinder3.height = 2.4; cylinder3.radius = cylinderRadius;
    cylinder3.rotation = identity;
    cylinder3.rotateFigY(90.0);

    float ballOffset = 0.4 + 1.0 + sin(1.7 * time);
    float ballRadius = 0.6;

    Sphere sphere1; sphere1.center = vec3(-ballOffset, 0.0, 0.0); sphere1.radius = ballRadius; sphere1.color =  vec4(1.0, 0.0, 0.0, 1.0);
    sphere1.rotation = identity;
    Sphere sphere2; sphere2.center = vec3(ballOffset, 0.0, 0.0); sphere2.radius = ballRadius; sphere2.color =  vec4(1.0, 0.0, 0.0, 1.0);
    sphere2.rotation = identity;
    Sphere sphere3; sphere3.center = vec3(0.0, -ballOffset, 0.0); sphere3.radius = ballRadius; sphere3.color =  vec4(1.0, 0.0, 0.0, 1.0);
    sphere3.rotation = identity;
    Sphere sphere4; sphere4.center = vec3(0.0, ballOffset, 0.0); sphere4.radius = ballRadius; sphere4.color =  vec4(1.0, 0.0, 0.0, 1.0);
    sphere4.rotation = identity;
    Sphere sphere5; sphere5.center = vec3(0.0, 0.0, -ballOffset); sphere5.radius = ballRadius; sphere5.color =  vec4(1.0, 0.0, 0.0, 1.0);
    sphere5.rotation = identity;
    Sphere sphere6; sphere6.center = vec3(0.0, 0.0, ballOffset); sphere6.radius = ballRadius; sphere6.color =  vec4(1.0, 0.0, 0.0, 1.0);
    sphere6.rotation = identity;

    float balls = sphere1.SDF(point);
    balls = unionSDF(balls, sphere2.SDF(point));
    balls = unionSDF(balls, sphere3.SDF(point));
    balls = unionSDF(balls, sphere4.SDF(point));
    balls = unionSDF(balls, sphere5.SDF(point));
    balls = unionSDF(balls, sphere6.SDF(point));
    
    float csgNut = differenceSDF(intersectSDF(box.SDF(point), sphere.SDF(point)),
                         unionSDF(cylinder1.SDF(point), unionSDF(cylinder2.SDF(point), cylinder3.SDF(point))));
    
    float intersection =  unionSDF(balls, csgNut);

    return min(intersection, plane.SDF(point));
}

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

vec3 getNormal(vec3 p){
    return normalize(vec3(
        sceneSDF(vec3(p.x + EPSILON, p.y, p.z)) - sceneSDF(vec3(p.x - EPSILON, p.y, p.z)),
        sceneSDF(vec3(p.x, p.y + EPSILON, p.z)) - sceneSDF(vec3(p.x, p.y - EPSILON, p.z)),
        sceneSDF(vec3(p.x, p.y, p.z  + EPSILON)) - sceneSDF(vec3(p.x, p.y, p.z - EPSILON))
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

vec3 phongContribForLight(vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye,
                          vec3 lightPos, vec3 lightIntensity) {
    vec3 N = getNormal(p);
    vec3 L = normalize(lightPos - p);
    vec3 V = normalize(eye - p);
    vec3 R = normalize(reflect(-L, N));
    
    float dotLN = dot(L, N);
    float dotRV = dot(R, V);
    
    if (dotLN < 0.0) {
        // Light not visible from this point on the surface
        return vec3(0.0, 0.0, 0.0);
    } 
    
    if (dotRV < 0.0) {
        // Light reflection in opposite direction as viewer, apply only diffuse
        // component
        return lightIntensity * (k_d * dotLN);
    }
    return lightIntensity * (k_d * dotLN + k_s * pow(dotRV, alpha));
}

vec3 phongIllumination(vec3 k_a, vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye) {
    const vec3 ambientLight = 0.5 * vec3(1.0, 1.0, 1.0);
    vec3 color = ambientLight * k_a;
    
    vec3 light1Pos = vec3(4.0 * sin(time),
                          2.0,
                          4.0 * cos(time));
    vec3 light1Intensity = vec3(0.4, 0.4, 0.4);
    
    color += phongContribForLight(k_d, k_s, alpha, p, eye,
                                  light1Pos,
                                  light1Intensity);
    
    vec3 light2Pos = vec3(2.0 * sin(0.37 * time),
                          2.0 * cos(0.37 * time),
                          2.0);
    vec3 light2Intensity = vec3(0.4, 0.4, 0.4);
    
    color += phongContribForLight(k_d, k_s, alpha, p, eye,
                                  light2Pos,
                                  light2Intensity);    
    return color;
}

void main()
{
    vec2 pixelCoord = gl_FragCoord.xy;
    vec2 uv = vec2(pixelCoord.x - (resolution.x/2.0), pixelCoord.y - (resolution.y/2.0))/1500.0;

    vec3 rayOrigin = cameraPosition;
    vec3 rayDir = normalize(vec3(uv.x + cameraDirection.x, uv.y + cameraDirection.y, cameraDirection.z));

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
    
    vec3 phongColor = phongIllumination(K_a, K_d, K_s, shininess, p, cameraPosition);

    vec3 lightColor = vec3(dif);
    gl_FragColor = vec4(phongColor * lightColor, 1.0);
}
