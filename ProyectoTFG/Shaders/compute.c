#version 430 core

layout  (local_size_x  =  2)  in;

const int COLLISION_SAMPLES = 100;
const float DAMPING = 0.1f;

layout(std430, binding=0) buffer Pos{
    float deltaTime;
    float radius;
    float mass;
    vec3 velocity;
    vec3 force;
    vec3 position;
    vec3 debug;
    vec3 collisionDirs[COLLISION_SAMPLES];
};

#include ..\\..\\Shaders\\snowTerrain.c

void main(){
    position += velocity * deltaTime;

    vec3 totalAcceleration = force / mass;
    velocity += totalAcceleration * deltaTime;
    velocity *= pow(DAMPING, deltaTime);

    // reset de las fuerzas
    force.x = force.y = force.z = 0;

    for(int i = 0; i < COLLISION_SAMPLES; i++){
        float dist = rayMarch(position, collisionDirs[i], MIN_DIST, MAX_DIST);

        if(dist < radius){
            position += abs(dist - radius) * -(collisionDirs[i]);
        }
    }
}