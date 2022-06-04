#version 420 core

// Positions/Coordinates
layout (location = 0) in vec4 aPos;
// Normals (not necessarily normalized)
layout (location = 1) in vec3 aNormal;
// Colors
layout (location = 2) in vec3 aColor;
// Texture Coordinates
layout (location = 3) in vec2 aTex;


// Outputs the current position for the Fragment Shader
out vec4 worldPos;
// Outputs the normal for the Fragment Shader
out vec4 normal;
// Outputs the color for the Fragment Shader
out vec3 color;
// Outputs the texture coordinates to the Fragment Shader
out vec2 texCoord;


struct Camera {
	vec3 position;
	mat4 viewProjMatrix;
	mat4 invViewProjMatrix;
};
uniform Camera camera;

struct SceneObject {
	mat4 modelMatrix;
	mat4 invModelMatrix;
};
uniform SceneObject sceneObject;

void main()
{
	worldPos = sceneObject.modelMatrix * aPos;
	normal = vec4(aNormal, 0.0) * sceneObject.invModelMatrix;
	color = aColor;
	texCoord = aTex;
	
	gl_Position = camera.viewProjMatrix * worldPos;
}