#version 450 core
#extension GL_ARB_separate_shader_objects : enable

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

float near = 0.1; 
float far  = 100.0; 

#include ..\\..\\Shaders\\geometries.c

void main()
{    
    gl_FragDepth = LinearizeDepth(gl_FragCoord.z)/far; // divide by far for demonstration
    FragColor = texture(texture_diffuse1, TexCoords);
}