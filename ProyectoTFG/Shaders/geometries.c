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
    vec4 color;

    float SDF(vec3 point){
        vec3 q = abs(point - center) - dimensions;
        return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
    }
};

