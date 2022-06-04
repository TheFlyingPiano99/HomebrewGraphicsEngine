#version 420 core

layout (location = 0) in vec3 aPos;

out vec3 modelPos;

struct Camera {
	mat4 viewProjMatrix;
};
uniform Camera lightCamera;

struct SceneObject {
	mat4 modelMatrix;
	mat4 invModelMatrix;
};
uniform SceneObject sceneObject;


void main()
{
	modelPos = aPos;
	gl_Position = lightCamera.viewProjMatrix * sceneObject.modelMatrix * vec4(aPos, 1.0);
}