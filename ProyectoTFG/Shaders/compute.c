#version 430 core

layout  (local_size_x  =  2)  in;

layout(std430, binding=0) buffer Pos{
    float deltaTime;
    float velocity;
	vec3 direction;
    vec3 position;
};

void main(){
    position += velocity * deltaTime * direction;
}