#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBitangent;
layout (location = 4) in vec2 aTex;
layout (location = 5) in mat4 modelMatrix;			// Instanced array
// location 6, 7, 8 included in the matrix
layout (location = 9)  in vec4 lightPosition;		// Instanced array
layout (location = 10) in vec4 lightPowerDensity;	// Instanced array
layout (location = 11) in float  shadowMapIdx;		// Instanced array	(-1 if no shadow mapping)

out VS_OUT {
vec4 ndc;
vec4 lightPosition;
vec3 lightPowerDensity;
float shadowMapIdx;
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
	vs_out.ndc = viewProjMatrix * modelMatrix * vec4(aPos, 1.0);
    gl_Position = vs_out.ndc;
	vs_out.lightPosition = lightPosition;
	vs_out.lightPowerDensity = lightPowerDensity.rgb;
	vs_out.shadowMapIdx = shadowMapIdx;
}