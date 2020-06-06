#version 450 core

layout  (local_size_x  =  2)  in;

const int COLLISION_SAMPLES = 200;

layout(std430, binding=0) buffer Pos{
    bool isGrounded;
    float deltaTime;
    float time;
    float radius;
    float mass;
    float damping;
    float airDamping;
    vec3 fractalRotation;
    vec3 velocity;
    vec3 force;
    vec3 position;
    vec3 debug;
    vec3 collisionDirs[COLLISION_SAMPLES];
};

#include ..\\..\\Shaders\\oceanTerrain.c

void main(){
    position += velocity * deltaTime;

    float currentDamping = damping;

    vec3 totalAcceleration = force / mass;
    velocity += totalAcceleration * deltaTime;
    if(!isGrounded) currentDamping = airDamping;
    velocity *= pow(airDamping, deltaTime);

    // reset de las fuerzas
    force.x = force.y = force.z = 0;

    isGrounded = false;
    for(int i = 0; i < COLLISION_SAMPLES; i++){
        float dist = rayMarch(position, collisionDirs[i], MIN_DIST, MAX_DIST, time)/28000;

        if(dist /*+ (radius/5)*/ < radius){
            isGrounded = true;
            position += abs(dist /*+ (radius/5)*/ - radius) * -(collisionDirs[i]);
        }
    }
}