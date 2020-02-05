#version 330 core

in vec3 aPos;
in vec2 uv0;

out vec2 texCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = /*projection * view **/ vec4(aPos, 1.0f);
	texCoords = uv0;
}