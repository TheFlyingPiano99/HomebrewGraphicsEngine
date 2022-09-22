#version 420 core

layout (location = 0) in vec3 aPos;

out vec3 modelPos;
out vec3 worldPos;

layout (std140, binding = 0) uniform Camera {	// base alignment	aligned offset
	vec3 cameraPosition;			// 16				0
	mat4 viewProjMatrix;			// 16				16				(column 0)
									// 16				32				(column 1)
									// 16				48				(column 2)
									// 16				64				(column 3)
	mat4 rayDirMatrix;				// 16				80				(column 0)
									// 16				96				(column 1)
									// 16				192				(column 2)
									// 16				208				(column 3)
};

struct SceneObject {
	mat4 modelMatrix;
	mat4 invModelMatrix;
};
uniform SceneObject sceneObject;

void main()
{
	modelPos = aPos;
	vec4 w_tempPos = sceneObject.modelMatrix * vec4(aPos, 1.0);
	worldPos = w_tempPos.xyz / w_tempPos.w;
	gl_Position = viewProjMatrix * w_tempPos;
}