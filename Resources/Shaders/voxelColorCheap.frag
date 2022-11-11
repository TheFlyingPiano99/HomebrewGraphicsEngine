#version 420 core

in vec3 modelPos;
in vec3 worldPos;

layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform sampler2D colorTexture;
layout (binding = 1) uniform sampler2D transferTexture;
layout (binding = 2) uniform sampler2D attenuationTexture;
layout (binding = 3) uniform sampler3D voxels;

uniform vec3 resolution;

struct SceneObject {
	mat4 modelMatrix;
	mat4 invModelMatrix;
};
uniform SceneObject sceneObject;

uniform float shininess;
uniform vec3 specularColor;
uniform vec3 ambientColor;
uniform bool isBackToFront;
uniform vec3 scale;
uniform vec3 w_sliceDelta;
uniform float opacityScale;
uniform bool usePBR;

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


vec4 transferFunctionFromTexture(vec3 pos) {
	vec4 gradientIntesity = resampleGradientAndDensity(pos, trilinearInterpolation(pos));
	return texture(transferTexture, vec2(gradientIntesity.w, length(gradientIntesity.xyz)));
}

vec4 transferFunctionFromTextureNoFilter(vec3 pos) {
	vec4 gradientIntesity = resampleGradientAndDensity(pos, texture(voxels, pos / resolution).r);
	return texture(transferTexture, vec2(gradientIntesity.w, length(gradientIntesity.xyz)));
}

float trilinearInterpolationFromTransfer(vec3 currentPos) {
	vec3 currentVoxel = vec3(ivec3(currentPos));
	vec3 inVoxelPos = currentPos - currentVoxel;
	float sample000 = length(transferFunctionFromTextureNoFilter((currentVoxel + vec3(0,0,0)) / resolution));
	float sample100 = length(transferFunctionFromTextureNoFilter((currentVoxel + vec3(1,0,0)) / resolution));
	float sample010 = length(transferFunctionFromTextureNoFilter((currentVoxel + vec3(0,1,0)) / resolution));
	float sample001 = length(transferFunctionFromTextureNoFilter((currentVoxel + vec3(0,0,1)) / resolution));
	float sample110 = length(transferFunctionFromTextureNoFilter((currentVoxel + vec3(1,1,0)) / resolution));
	float sample101 = length(transferFunctionFromTextureNoFilter((currentVoxel + vec3(1,0,1)) / resolution));
	float sample011 = length(transferFunctionFromTextureNoFilter((currentVoxel + vec3(0,1,1)) / resolution));
	float sample111 = length(transferFunctionFromTextureNoFilter((currentVoxel + vec3(1,1,1)) / resolution));

	return (
				(sample000 * (1.0 - inVoxelPos.z) + sample001 * inVoxelPos.z) * (1.0 - inVoxelPos.y)
				+ (sample010 * (1.0 - inVoxelPos.z) + sample011 * inVoxelPos.z) * inVoxelPos.y
			) * (1.0 - inVoxelPos.x)
		 + (
				(sample100 * (1.0 - inVoxelPos.z) + sample101 * inVoxelPos.z) * (1.0 - inVoxelPos.y)
				+ (sample110 * (1.0 - inVoxelPos.z) + sample111 * inVoxelPos.z) * inVoxelPos.y
		   ) * inVoxelPos.x;
}


vec4 resampleGradientAndDensityFromTransfer(vec3 position, float intensity)
{
	vec3 normPos = position / resolution;
	vec3 stepSize = 1.0 / resolution;
	vec3 sample0, sample1;
	sample0.x = length(transferFunctionFromTextureNoFilter(vec3(normPos.x - stepSize.x, normPos.y, normPos.z) * resolution));
	sample1.x = length(transferFunctionFromTextureNoFilter(vec3(normPos.x + stepSize.x, normPos.y, normPos.z) * resolution));
	sample0.y = length(transferFunctionFromTextureNoFilter(vec3(normPos.x, normPos.y - stepSize.y, normPos.z) * resolution));
	sample1.y = length(transferFunctionFromTextureNoFilter(vec3(normPos.x, normPos.y + stepSize.y, normPos.z) * resolution));
	sample0.z = length(transferFunctionFromTextureNoFilter(vec3(normPos.x, normPos.y, normPos.z - stepSize.z) * resolution));
	sample1.z = length(transferFunctionFromTextureNoFilter(vec3(normPos.x, normPos.y, normPos.z + stepSize.z) * resolution));
	vec3 scaledPosition = position * resolution - 0.5;
	vec3 fraction = scaledPosition - floor(scaledPosition);
	vec3 correctionPolynomial = (fraction * (fraction - 1.0)) / 2.0;
	intensity += dot((sample0 - intensity * 2.0 + sample1),
	correctionPolynomial);
	return vec4(sample1 - sample0, intensity);
}


void main() {	
	vec3 w_viewDir = normalize(cameraPosition - worldPos);
	float w_delta = opacityScale * length(w_sliceDelta) / abs(dot(normalize(w_sliceDelta), w_viewDir));
	vec3 currentPos = modelPos + resolution * 0.5;
	vec4 ndc_temp = viewProjMatrix * vec4(worldPos, 1);
	ndc_temp /= ndc_temp.w;
	vec2 cameraTexCoords = ndc_temp.xy * 0.5 + 0.5;
	vec4 prevColor = texture(colorTexture, cameraTexCoords);

	vec4 gradIntensity = resampleGradientAndDensity(currentPos, texture(voxels, currentPos / resolution).r);
	vec4 color = texture(transferTexture, vec2(gradIntensity.w, length(gradIntensity.xyz)));

	// Calculate opacity of this segment:
	float w_lightDistance = length(light.position - worldPos);
	color.rgb *= light.powerDensity / w_lightDistance / w_lightDistance * w_delta;
	
	vec4 ndc_attenuationCoords = light.viewProjMatrix * vec4(worldPos, 1.0);
	ndc_attenuationCoords /= ndc_attenuationCoords.w;
	vec2 attenuationTexCoords = ndc_attenuationCoords.xy * 0.5 + 0.5;
	vec4 attenuation = texture(attenuationTexture, attenuationTexCoords);

	// Light Attenuation from lights direction:
	vec3 l = 1.0 - attenuation.rgb - attenuation.a;
	color.rgb *= vec3(max(l.x, 0.0), max(l.y, 0.0), max(l.z, 0.0));
	color.a = max(min(1.0 - pow(1.0 - color.a, w_delta), 1.0), 0.0);
	FragColor = color;
}