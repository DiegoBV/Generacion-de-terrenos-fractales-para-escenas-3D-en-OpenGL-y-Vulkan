#version 430 core

layout  (local_size_x  =  2)  in;

layout(std430, binding=0) buffer Pos{
    vec3 position;
};

void main(){
    position.x = position.x + 0.1;
}