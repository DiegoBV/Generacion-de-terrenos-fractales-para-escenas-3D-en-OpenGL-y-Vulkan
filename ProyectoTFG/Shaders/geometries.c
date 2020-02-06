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

    float SDF(vec3 point) {
        return length(point - center) - radius;
    }

    void rotateFigX(float angle){
        rotation = rotateX(angle, rotation);
    }
    void rotateFigY(float angle){
        rotation = rotateY(angle, rotation);
    }
    void rotateFigZ(float angle){
        rotation = rotateZ(angle, rotation);
    }
};

struct Box{
    vec3 dimensions;
    vec3 center;
    mat3 rotation;
    vec4 color;

    float SDF(vec3 point){
        point = point * rotation;
        vec3 q = (abs(point - center)) - dimensions;
        return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
    }

    void rotateFigX(float angle){
        rotation = rotateX(angle, rotation);
    }
    void rotateFigY(float angle){
        rotation = rotateY(angle, rotation);
    }
    void rotateFigZ(float angle){
        rotation = rotateZ(angle, rotation);
    }
};

struct Cylinder{
    vec3 center;
    float radius;
    float height;
    mat3 rotation;
    vec4 color;

    float SDF(vec3 point) {
        point = (point - center) * rotation;
        vec2 d = abs(vec2(length(point.xy),point.z)) - vec2(radius,height);
        return min(max(d.x,d.y),0.0) + length(max(d,0.0));
    }

    void rotateFigX(float angle){
        rotation = rotateX(angle, rotation);
    }
    void rotateFigY(float angle){
        rotation = rotateY(angle, rotation);
    }
    void rotateFigZ(float angle){
        rotation = rotateZ(angle, rotation);
    }
};

struct Plane{
    float height;

    float SDF(vec3 point){
        return point.y + height;
    }
};