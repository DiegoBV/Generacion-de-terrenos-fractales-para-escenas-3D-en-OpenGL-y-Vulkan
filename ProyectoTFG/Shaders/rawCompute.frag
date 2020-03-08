#version 430 core

layout  (local_size_x  =  2)  in;

layout(std430, binding=0) writeonly buffer Pos{
    float Position[];
};

void main(){
    Position[gl_GlobalInvocationID.x] = 8.0f;
}
