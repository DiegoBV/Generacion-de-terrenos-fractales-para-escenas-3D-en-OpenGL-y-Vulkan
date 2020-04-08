// GEOMETRIES

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

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}