struct Sphere{
    float radius;
    vec3 center;
    vec4 color;

    float SDF(vec3 point) {
        return length(point - center) - radius;
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

    void rotateX(float angle){
        angle = radians(angle);
        rotation = mat3(
        1, 0, 0,
        0, cos(angle), -sin(angle),
        0, sin(angle), cos(angle)  
        );
    }
    void rotateY(float angle){
        angle = radians(angle);
        rotation = mat3(
        cos(angle), 0, sin(angle),
        0, 1, 0, 
        -sin(angle), 0, cos(angle) 
        );
    }
    void rotateZ(float angle){
        angle = radians(angle);
        rotation = mat3(
        cos(angle), -sin(angle), 0,
        sin(angle), cos(angle), 0, 
        0, 0, 1
        );
    }
};

