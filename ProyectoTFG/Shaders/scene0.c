vec2 SDF(vec3 point);
float rayMarch(vec3 eye, vec3 marchingDirection, float start, float end);

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

vec2 SDF(vec3 point){
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

    return vec2(min(intersection, plane.SDF(point)), 1.0);
}

float rayMarch(vec3 eye, vec3 marchingDirection, float start, float end){
    float depth = start;

    for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
        vec2 dist = SDF(eye + depth * marchingDirection);
        if (dist.x < EPSILON) {
			break;
        }
        depth += MARCHINGSTEP * dist.x;
        if (depth >= end) {
            break;
        }
    }

    return depth;
}

vec3 getColor(vec3 p){        
    float dif = getLight(p);
    
    vec3 K_a = (getNormal(p) + vec3(1.0)) / 2.0;
    vec3 K_d = K_a;
    vec3 K_s = vec3(1.0, 1.0, 1.0);
    float shininess = 10.0;
    
    vec3 phongColor = phongIllumination(K_a, K_d, K_s, shininess, p, cameraEye);

    vec3 lightColor = vec3(dif);

    return lightColor * phongColor;
}