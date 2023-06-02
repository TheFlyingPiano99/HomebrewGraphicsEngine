#version 420 core

/*
Vertex shader for fulscreen quads with additional rayDir output.
*/

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inTex;

out VS_OUT {
vec2 texCoords;
vec4 rayDir;
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
    vs_out.texCoords = inTex;
    vs_out.rayDir = rayDirMatrix * vec4(inPos, 0.0, 1.0);
    gl_Position = vec4(inPos, 0.0, 1.0);
}