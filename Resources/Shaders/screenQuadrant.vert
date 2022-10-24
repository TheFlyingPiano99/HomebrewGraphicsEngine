#version 420 core

layout (location = 0) in vec2 inPos;

out vec2 texCoords;
out vec4 w_rayDir;

uniform mat4 quadModelMatrix;

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

void main() {
	vec4 ndc_pos = quadModelMatrix * vec4(inPos.x, inPos.y, 0.0, 1.0);
	ndc_pos /= ndc_pos.w;
	texCoords = (ndc_pos.xy + 1.0) / 2.0;
	w_rayDir = rayDirMatrix * ndc_pos;
	w_rayDir /= w_rayDir.w;
	gl_Position = ndc_pos;
}
