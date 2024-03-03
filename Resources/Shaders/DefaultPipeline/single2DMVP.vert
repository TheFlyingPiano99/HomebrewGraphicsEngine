#version 460 core
/*
*   Vertex shader for rendering simple 2D sprites in screen-space
*   No depth information is considered
*/ 

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inTex;

out vec2 texCoords;

struct SceneObject {
	mat4 modelMatrix;	
	mat4 invModelMatrix;
};
uniform SceneObject sceneObject;

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
} camera;

void main()
{
    texCoords = inTex;
    gl_Position = camera.viewProjMatrix * sceneObject.modelMatrix * vec4(inPos.xy, 0.0, 1.0);
}