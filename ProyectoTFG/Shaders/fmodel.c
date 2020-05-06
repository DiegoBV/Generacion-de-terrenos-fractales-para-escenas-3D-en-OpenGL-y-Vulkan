#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 TexCoords;

layout(binding = 1) uniform sampler2D texture_diffuse1;

float near = 0.1; 
float far  = 100.0; 

#include ..\\..\\Shaders\\geometries.c

void main()
{    
    vec2 texCoords = TexCoords;
    #ifdef VULKAN
        texCoords.y = 1.0 - texCoords.y;
        gl_FragDepth = LinearizeDepth((gl_FragCoord.z + 1)/2)/far; // divide by far for demonstration
    #else
        gl_FragDepth = LinearizeDepth(gl_FragCoord.z)/far; // divide by far for demonstration
    #endif
    
    outColor = texture(texture_diffuse1, texCoords);
}