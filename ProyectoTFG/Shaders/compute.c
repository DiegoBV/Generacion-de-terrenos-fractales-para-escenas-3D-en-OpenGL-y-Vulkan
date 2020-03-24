#version 430 core

layout  (local_size_x  =  2)  in;

const int COLLISION_SAMPLES = 100;

layout(std430, binding=0) buffer Pos{
    float deltaTime;
    float velocity;
    float radius;
    vec3 gravity;
    vec3 direction;
    vec3 position;
    vec3 debug;
    vec3 collisionDirs[COLLISION_SAMPLES];
};

#include ..\\..\\Shaders\\snowTerrain.c

void main(){
    position += velocity * deltaTime * direction;
    position += deltaTime * gravity; // gravedad

    for(int i = 0; i < COLLISION_SAMPLES; i++){
        float dist = rayMarch(position, collisionDirs[i], MIN_DIST, MAX_DIST);

        if(dist < radius){
            position += abs(dist - radius) * -(collisionDirs[i]);
        }
    }
}