#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBitangent;
layout (location = 4) in vec2 aTex;
layout (location = 5) in mat4 modelMatrix;		// Instanced array


layout (std140, binding = 2) uniform ShadowCaster {
	mat4 lightSpaceMatrix;
};

void main()
{
	gl_Position = lightSpaceMatrix * modelMatrix * vec4(aPos, 1.0);
}