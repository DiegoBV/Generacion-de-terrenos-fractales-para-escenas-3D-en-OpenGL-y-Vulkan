#version 330 core

in vec3 aPos;
in vec2 uv0;

out vec2 texCoords;

void main()
{
	gl_Position = vec4(aPos, 1.0f);
	texCoords = uv0;
}