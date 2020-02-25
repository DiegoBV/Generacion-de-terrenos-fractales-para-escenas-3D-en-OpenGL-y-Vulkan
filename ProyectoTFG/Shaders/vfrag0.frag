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

layout(location = 0) out vec4 outColor;

mat3 rotateX(float angle, mat3 rotation)
{
    angle = radians(angle);
    rotation = mat3(
        1, 0, 0,
        0, cos(angle), -sin(angle),
        0, sin(angle), cos(angle));
    return rotation;
}
mat3 rotateY(float angle, mat3 rotation)
{
    angle = radians(angle);
    rotation = mat3(
        cos(angle), 0, sin(angle),
        0, 1, 0,
        -sin(angle), 0, cos(angle));
    return rotation;
}
mat3 rotateZ(float angle, mat3 rotation)
{
    angle = radians(angle);
    rotation = mat3(
        cos(angle), -sin(angle), 0,
        sin(angle), cos(angle), 0,
        0, 0, 1);
    return rotation;
}


struct Sphere{
    float radius;
    vec3 center;
    mat3 rotation;
    vec4 color;
};

float sphereSDF(Sphere sphere, vec3 point) {
    return length(point - sphere.center) - sphere.radius;
}

struct Box{
    vec3 dimensions;
    vec3 center;
    mat3 rotation;
    vec4 color;
};

float boxSDF(Box box, vec3 point){
    point = point * box.rotation;
    vec3 q = (abs(point - box.center)) - box.dimensions;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

struct Cylinder{
    vec3 center;
    float radius;
    float height;
    mat3 rotation;
    vec4 color;
};

float cylinderSDF(Cylinder cylinder, vec3 point) {
    point = (point - cylinder.center) * cylinder.rotation;
    vec2 d = abs(vec2(length(point.xy),point.z)) - vec2(cylinder.radius, cylinder.height);
    return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

struct Plane{
    float height;
};

float planeSDF(Plane plane, vec3 point){
    return point.y + plane.height;
}

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

float getDistance(vec3 point){
    Sphere sphere;
    sphere.center = vec3(1.0, 0.0, 0.0);
    sphere.radius = 3.0;
    sphere.color = vec4(1.0, 0.0, 0.0, 1.0);

    return min(sphereSDF(sphere, point), point.y + 5.0);
}

float sceneSDF(vec3 point){
    mat3 identity = mat3(1, 0, 0, 0, 1, 0, 0, 0, 1);
    point = rotateY(degrees(ubo.time / 2.0), identity) * point;

    Plane plane; plane.height = 5.0;

    Sphere sphere; sphere.center = vec3(0.0, 0.0, 0.0); sphere.radius = 2.4; sphere.color = vec4(1.0, 0.0, 0.0, 1.0);
    sphere.rotation = identity;
    Box box; box.center = vec3(0.0, 0.0, 0.0); box.dimensions = vec3(2, 2, 2); box.color = vec4(0.0, 1.0, 0.0, 1.0);
    box.rotation = identity;

    float cylinderRadius = 0.8 + (1.0 - 0.4) * (3.0 + sin(1.7 * ubo.time)) / 2.0;
    Cylinder cylinder1; cylinder1.center = vec3(0.0, 0.0, 0.0); cylinder1.height = 2.4; cylinder1.radius = cylinderRadius;
    cylinder1.rotation = identity;
    Cylinder cylinder2; cylinder2.center = vec3(0.0, 0.0, 0.0); cylinder2.height = 2.4; cylinder2.radius = cylinderRadius;
    cylinder2.rotation = identity;
    cylinder2.rotation = rotateX(90.0, cylinder2.rotation);
    Cylinder cylinder3; cylinder3.center = vec3(0.0, 0.0, 0.0); cylinder3.height = 2.4; cylinder3.radius = cylinderRadius;
    cylinder3.rotation = identity;
    cylinder3.rotation = rotateY(90.0, cylinder3.rotation);

    float ballOffset = 0.4 + 1.0 + sin(1.7 * ubo.time);
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

    float balls = sphereSDF(sphere1, point);
    balls = unionSDF(balls, sphereSDF(sphere2, point));
    balls = unionSDF(balls, sphereSDF(sphere3, point));
    balls = unionSDF(balls, sphereSDF(sphere4, point));
    balls = unionSDF(balls, sphereSDF(sphere5, point));
    balls = unionSDF(balls, sphereSDF(sphere6, point));
    
    float csgNut = differenceSDF(intersectSDF(boxSDF(box, point), sphereSDF(sphere, point)),
                         unionSDF(cylinderSDF(cylinder1, point), unionSDF(cylinderSDF(cylinder2, point), cylinderSDF(cylinder3, point))));
    
    float intersection =  unionSDF(balls, csgNut);

    return min(intersection, planeSDF(plane, point));
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
    lightPos.x += cos(ubo.time)*5.0;
    lightPos.z += sin(ubo.time)*5.0;
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
    
    vec3 light1Pos = vec3(4.0 * sin(ubo.time),
                          2.0,
                          4.0 * cos(ubo.time));
    vec3 light1Intensity = vec3(0.4, 0.4, 0.4);
    
    color += phongContribForLight(k_d, k_s, alpha, p, eye,
                                  light1Pos,
                                  light1Intensity);
    
    vec3 light2Pos = vec3(2.0 * sin(0.37 * ubo.time),
                          2.0 * cos(0.37 * ubo.time),
                          2.0);
    vec3 light2Intensity = vec3(0.4, 0.4, 0.4);
    
    color += phongContribForLight(k_d, k_s, alpha, p, eye,
                                  light2Pos,
                                  light2Intensity);    
    return color;
}

vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord) {
    vec2 uv = vec2(fragCoord.x - (size.x/2.0), fragCoord.y - (size.y/2.0));
    uv.y = -uv.y;
    float z = size.y / tan(radians(fieldOfView) / 2.0);
    return normalize(vec3(uv, -z));
}

void main()
{
    vec2 pixelCoord = gl_FragCoord.xy;
    vec3 rayOrigin = ubo.cameraEye;
    vec3 rayDir = rayDirection(45.0, ubo.resolution, pixelCoord);
    rayDir = (ubo.viewMat * vec4(rayDir, 0.0)).xyz; //works
    float distanceToSurface = rayMarch(rayOrigin, rayDir, MIN_DIST, MAX_DIST);

    vec3 p = rayOrigin + rayDir * distanceToSurface;

    float dif = getLight(p);

    if (distanceToSurface > MAX_DIST - EPSILON) {
        // Didn't hit anything
        outColor = vec4(0.1, 0.1, 0.1, 1.0);
		return;
    }

    // Use the surface normal as the ambient color of the material
    vec3 K_a = (getNormal(p) + vec3(1.0)) / 2.0;
    vec3 K_d = K_a;
    vec3 K_s = vec3(1.0, 1.0, 1.0);
    float shininess = 10.0;
    
    vec3 phongColor = phongIllumination(K_a, K_d, K_s, shininess, p, ubo.cameraEye);

    vec3 lightColor = vec3(dif);
    outColor = vec4(phongColor * lightColor, 1.0);
}
