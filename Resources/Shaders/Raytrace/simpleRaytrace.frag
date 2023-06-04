#version 430

in VS_OUT {
	vec2 texCoords;
	vec4 rayDir;
} vs_out;

out vec4 FragColor;

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



layout(binding = 0) uniform sampler3D voxels;

void main()
{
	vec3 pos = cameraPosition;
	float stepSize = 0.015;
	float intensity = 0;
	vec3 dir = normalize(vs_out.rayDir.xyz);
	for (int i = 0; i < 300; i++) {
		vec2 waveFunc = texture(voxels, (pos + 1.0) / 2.0).xy;	// Sample are is in the [-10, 10] area in each direction.
		float probability = clamp(waveFunc.x * waveFunc.x + waveFunc.y * waveFunc.y, 0, 1) * stepSize;
		intensity = intensity + probability * (1 - intensity);	// Under operator
		pos += dir * stepSize;
	}
	FragColor = vec4(intensity.xxx, 1);
}
