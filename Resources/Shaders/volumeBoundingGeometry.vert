#version 420 core

layout (location = 0) in vec3 aPos;

out vec3 modelPos;

struct Camera {
	mat4 viewProjMatrix;
};
uniform Camera camera;

struct SceneObject {
	mat4 modelMatrix;
};
uniform SceneObject sceneObject;


void main()
{
	modelPos = aPos;
	gl_Position = camera.viewProjMatrix * sceneObject.modelMatrix * vec4(aPos, 1.0);
}