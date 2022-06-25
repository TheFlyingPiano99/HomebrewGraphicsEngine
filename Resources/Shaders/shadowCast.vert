#version 420 core

// Positions/Coordinates
layout (location = 0) in vec4 aPos;
// Normals (not necessarily normalized)
layout (location = 1) in vec3 aNormal;
// Colors
layout (location = 2) in vec3 aColor;
// Texture Coordinates
layout (location = 3) in vec2 aTex;

uniform struct SceneObject {
	mat4 modelMatrix;
} sceneObject;

layout (std140, binding = 2) uniform ShadowCaster {
	mat4 lightSpaceMatrix;
};

void main()
{
	gl_Position = lightSpaceMatrix * sceneObject.modelMatrix * aPos;
}