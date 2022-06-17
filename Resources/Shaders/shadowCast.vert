#version 420 core

// Positions/Coordinates
layout (location = 0) in vec4 aPos;
// Normals (not necessarily normalized)
layout (location = 1) in vec3 aNormal;
// Colors
layout (location = 2) in vec3 aColor;
// Texture Coordinates
layout (location = 3) in vec2 aTex;

struct SceneObject {
	mat4 modelMatrix;
};
uniform SceneObject sceneObject;

struct ShadowCaster {
	mat4 lightSpaceMatrix;
};
uniform ShadowCaster shadowCaster;

void main()
{
	gl_Position = shadowCaster.lightSpaceMatrix * sceneObject.modelMatrix * aPos;
}