#version 420 core

layout (location = 0) out vec4 FragColor;

in vec3 modelPos;

void main()
{
	FragColor = vec4(modelPos, 1.0);
}