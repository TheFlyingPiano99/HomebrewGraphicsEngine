#version 420 core

in vec3 modelPos;
in vec3 worldPos;

layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform sampler2D colorTexture;
layout (binding = 1) uniform sampler2D transferTexture;
layout (binding = 2) uniform sampler2D attenuationTexture;
layout (binding = 3) uniform sampler3D voxels;

uniform vec3 resolution;
uniform bool isBackToFront;

struct SceneObject {
	mat4 modelMatrix;
	mat4 invModelMatrix;
};
uniform SceneObject sceneObject;

uniform float shininess;
uniform vec3 specularColor;
uniform vec3 ambientColor;
uniform vec3 scale;
uniform vec3 w_sliceDelta;
uniform float opacityScale;

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

struct Light {
	vec3 position;
	vec3 powerDensity;
	mat4 viewProjMatrix;
};
uniform Light light;

float rand(vec2 n) { 
	return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

float noise(vec2 n) {
	const vec2 d = vec2(0.0, 1.0);
  vec2 b = floor(n), f = smoothstep(vec2(0.0), vec2(1.0), fract(n));
	return mix(mix(rand(b), rand(b + d.yx), f.x), mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
}

float trilinearInterpolation(vec3 currentPos) {
	vec3 currentVoxel = vec3(ivec3(currentPos));
	vec3 inVoxelPos = currentPos - currentVoxel;
	float sample000 = texture(voxels, (currentVoxel + vec3(0,0,0)) / resolution).r;
	float sample100 = texture(voxels, (currentVoxel + vec3(1,0,0)) / resolution).r;
	float sample010 = texture(voxels, (currentVoxel + vec3(0,1,0)) / resolution).r;
	float sample001 = texture(voxels, (currentVoxel + vec3(0,0,1)) / resolution).r;
	float sample110 = texture(voxels, (currentVoxel + vec3(1,1,0)) / resolution).r;
	float sample101 = texture(voxels, (currentVoxel + vec3(1,0,1)) / resolution).r;
	float sample011 = texture(voxels, (currentVoxel + vec3(0,1,1)) / resolution).r;
	float sample111 = texture(voxels, (currentVoxel + vec3(1,1,1)) / resolution).r;

	float filtered = (
				(sample000 * (1.0 - inVoxelPos.z) + sample001 * inVoxelPos.z) * (1.0 - inVoxelPos.y)
				+ (sample010 * (1.0 - inVoxelPos.z) + sample011 * inVoxelPos.z) * inVoxelPos.y
			) * (1.0 - inVoxelPos.x)
		 + (
				(sample100 * (1.0 - inVoxelPos.z) + sample101 * inVoxelPos.z) * (1.0 - inVoxelPos.y)
				+ (sample110 * (1.0 - inVoxelPos.z) + sample111 * inVoxelPos.z) * inVoxelPos.y
		   ) * inVoxelPos.x;

	return filtered;
}


vec4 resampleGradientAndDensity(vec3 position, float intensity)
{
	vec3 normPos = position / resolution;
	vec3 stepSize = 1.0 / resolution;
	vec3 sample0, sample1;
	sample0.x = texture(voxels,
	vec3(normPos.x - stepSize.x, normPos.y, normPos.z)).r;
	sample1.x = texture(voxels,
	vec3(normPos.x + stepSize.x, normPos.y, normPos.z)).r;
	sample0.y = texture(voxels,
	vec3(normPos.x, normPos.y - stepSize.y, normPos.z)).r;
	sample1.y = texture(voxels,
	vec3(normPos.x, normPos.y + stepSize.y, normPos.z)).r;
	sample0.z = texture(voxels,
	vec3(normPos.x, normPos.y, normPos.z - stepSize.z)).r;
	sample1.z = texture(voxels,
	vec3(normPos.x, normPos.y, normPos.z + stepSize.z)).r;
	vec3 scaledPosition = position * resolution - 0.5;
	vec3 fraction = scaledPosition - floor(scaledPosition);
	vec3 correctionPolynomial = (fraction * (fraction - 1.0)) / 2.0;
	intensity += dot((sample0 - intensity * 2.0 + sample1),
	correctionPolynomial);
	return vec4(sample1 - sample0, intensity);
}


vec4 transferFunctionFromTexture(float i, float g) {
	return texture(transferTexture, vec2(i, g));
}

void main() {	

	vec3 viewDir = normalize(cameraPosition - worldPos);
	float w_delta = length(w_sliceDelta) / abs(dot(normalize(w_sliceDelta), viewDir));
	vec3 currentPos = modelPos + resolution * 0.5;
	vec4 gradientIntesity = resampleGradientAndDensity(currentPos, texture(voxels, currentPos / resolution).r);
	vec4 color = transferFunctionFromTexture(gradientIntesity.w, length(gradientIntesity.xyz));
	
	vec4 ndc_attenuationCoords = light.viewProjMatrix * vec4(worldPos, 1.0);
	ndc_attenuationCoords /= ndc_attenuationCoords.w;
	vec2 texCoords = ndc_attenuationCoords.xy * 0.5 + 0.5;

<<<<<<< HEAD
	float offset = w_delta * tan(0.01 / 2.0);
	vec3 bluredIndirectAttenuation = vec3(0, 0, 0);	
=======
	float theta = 0.05;
	float offset = w_delta * tan(theta / 2.0);
	vec3 bluredIndirectAttenuation = vec3(0, 0, 0);
>>>>>>> attenuation-blur
	bluredIndirectAttenuation.rgb += texture(attenuationTexture, texCoords).rgb;
	bluredIndirectAttenuation.rgb += texture(attenuationTexture, texCoords + vec2(offset, 0)).rgb;
	bluredIndirectAttenuation.rgb += texture(attenuationTexture, texCoords + vec2(-offset, 0)).rgb;
	bluredIndirectAttenuation.rgb += texture(attenuationTexture, texCoords + vec2(0, offset)).rgb;
	bluredIndirectAttenuation.rgb += texture(attenuationTexture, texCoords + vec2(0, -offset)).rgb;
	bluredIndirectAttenuation.rgb += texture(attenuationTexture, texCoords + 0.707 * vec2(offset, offset)).rgb;
	bluredIndirectAttenuation.rgb += texture(attenuationTexture, texCoords + 0.707 * vec2(-offset, offset)).rgb;
	bluredIndirectAttenuation.rgb += texture(attenuationTexture, texCoords + 0.707 * vec2(offset, -offset)).rgb;
	bluredIndirectAttenuation.rgb += texture(attenuationTexture, texCoords + 0.707 * vec2(-offset, -offset)).rgb;
	bluredIndirectAttenuation /= 9;
	
	vec3 currentIndirectAttenuation = color.a * vec3(0.3, 0.9, 1.0);
	currentIndirectAttenuation.r = min(max(1.0 - pow(1.0 - currentIndirectAttenuation.r, opacityScale * w_delta), 0.0), 1.0);
	currentIndirectAttenuation.g = min(max(1.0 - pow(1.0 - currentIndirectAttenuation.g, opacityScale * w_delta), 0.0), 1.0);
	currentIndirectAttenuation.b = min(max(1.0 - pow(1.0 - currentIndirectAttenuation.b, opacityScale * w_delta), 0.0), 1.0);
	vec3 blendedIndirect = currentIndirectAttenuation * (vec3(1.0) - bluredIndirectAttenuation) + bluredIndirectAttenuation;
	
	// Calculate attenuation:
	FragColor = vec4(
		blendedIndirect,
		min(max(1.0 - pow(1.0 - color.a, opacityScale * w_delta), 0.0), 1.0)
	);
}