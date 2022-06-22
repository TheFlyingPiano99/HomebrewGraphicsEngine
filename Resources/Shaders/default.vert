#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBitangent;
layout (location = 4) in vec2 aTex;

out vec4 worldPos;
out vec4 lightPos;
out vec2 texCoords;
out mat3 TBN;	// Tangent-Bitangent-Normal matrix


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

struct ShadowCaster {
	mat4 lightSpaceMatrix;
};
uniform ShadowCaster shadowCaster;

void main()
{
	worldPos = sceneObject.modelMatrix * vec4(aPos, 1.0);
	lightPos = shadowCaster.lightSpaceMatrix * worldPos;
	texCoords = aTex;	
	vec3 wTangent = (vec4(aTangent, 0.0) * sceneObject.invModelMatrix).xyz;
	vec3 wBitangent = (vec4(aBitangent, 0.0) * sceneObject.invModelMatrix).xyz;
	vec3 wNormal = (vec4(aNormal, 0.0) * sceneObject.invModelMatrix).xyz;
	TBN = mat3(wTangent, wBitangent, wNormal);
	gl_Position = camera.viewProjMatrix * worldPos;
}