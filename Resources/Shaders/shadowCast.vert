#version 420 core

layout (location = 0) in vec3 aPos;
// ...
layout (location = 5) in mat4 modelMatrix;		// Instanced array

uniform	mat4 lightSpaceMatrix;

void main()
{
	gl_Position = lightSpaceMatrix * modelMatrix * vec4(aPos, 1.0);
}