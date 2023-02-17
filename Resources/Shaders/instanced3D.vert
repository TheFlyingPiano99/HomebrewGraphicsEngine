#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

layout (location = 5) in mat4 modelMatrix;		// Instanced array
layout (location = 9) in mat4 invModelMatrix;	// Instanced array 

out VS_OUT {
	vec4 worldPos;
	vec2 texCoords;
} vs_out;

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

void main()
{
	vs_out.worldPos = modelMatrix * vec4(aPos, 1.0);
	vs_out.texCoords = aTex;
	gl_Position = viewProjMatrix * vs_out.worldPos;
}
