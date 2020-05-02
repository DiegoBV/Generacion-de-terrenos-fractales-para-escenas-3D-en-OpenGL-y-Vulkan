float SDF(vec3 point, float time);
float rayMarch(vec3 eye, vec3 marchingDirection, float start, float end, float time);

const int MAX_MARCHING_STEPS = 2500;
const float MIN_DIST = 0.0;
const float MAX_DIST = 1000.0;
const float EPSILON = 0.0001;
const float  MARCHINGSTEP = 1.0;

#include ..\\..\\Shaders\\geometries.c
#include ..\\..\\Shaders\\lightning.c

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

float SDF(vec3 point, float time){
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
    cylinder2.rotation = rotateX(90.0, cylinder2.rotation);
    Cylinder cylinder3; cylinder3.center = vec3(0.0, 0.0, 0.0); cylinder3.height = 2.4; cylinder3.radius = cylinderRadius;
    cylinder3.rotation = identity;
    cylinder3.rotation = rotateY(90.0, cylinder3.rotation);

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

float rayMarch(vec3 eye, vec3 marchingDirection, float start, float end, float time){
    float depth = start;

    for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
        float dist = SDF(eye + depth * marchingDirection, time);
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