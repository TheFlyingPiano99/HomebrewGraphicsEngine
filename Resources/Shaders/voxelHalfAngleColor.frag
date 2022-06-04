#version 420 core

/*
	Half angle method
*/

in vec3 modelPos;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragColorAttenuation;

layout (binding = 0) uniform sampler3D voxels;
layout (binding = 1) uniform sampler2D colorAttenuationTransfer;
layout (binding = 2) uniform sampler2D enterTexture;
layout (binding = 3) uniform sampler2D exitTexture;
layout (binding = 4) uniform sampler2D opacityTexture;
layout (binding = 5) uniform samplerCube skybox;

layout (binding = 7) uniform sampler2D lightExitTexture0;
layout (binding = 8) uniform sampler2D lightExitTexture1;
layout (binding = 9) uniform sampler2D lightExitTexture2;
layout (binding = 10) uniform sampler2D lightExitTexture3;
layout (binding = 11) uniform sampler2D lightExitTexture4;
layout (binding = 12) uniform sampler2D lightExitTexture5;
layout (binding = 13) uniform sampler2D lightExitTexture6;

uniform vec3 resolution;

struct SceneObject {
	mat4 modelMatrix;
	mat4 invModelMatrix;
};
uniform SceneObject sceneObject;

uniform float shininess;
uniform vec3 specularColor;
uniform vec3 ambientColor;

struct Plane {
	vec3 position;
	vec3 normal;
};
uniform Plane worldCrossSectionPlane;	// <
uniform Plane modelHalfwaySlicePlane;	// <	- The two planes are stored in different spaces!!!
uniform float modelHalfwaySlicePlaneDelta;

uniform float noiseScale;

struct Camera {
	mat4 viewProjMatrix;
};
uniform Camera viewCamera;
uniform Camera lightCamera;

struct Light {
	vec4 position;
	vec3 powerDensity;
	mat4 viewProjMatrix;
};
uniform Light lights[16];
uniform unsigned int lightCount;

float rand(vec2 n) { 
	return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

float noise(vec2 n) {
	const vec2 d = vec2(0.0, 1.0);
  vec2 b = floor(n), f = smoothstep(vec2(0.0), vec2(1.0), fract(n));
	return mix(mix(rand(b), rand(b + d.yx), f.x), mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
}

vec3 intersectPlane(vec3 linePos, vec3 lineDir, vec3 planePos, vec3 planeNormal) {
	return linePos + lineDir * dot(planePos - linePos, planeNormal) / dot(lineDir, planeNormal);
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


vec4 resampleGradientAndIntensity(vec3 position, float intensity)
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


vec3 BlinnPhong(
	vec3 diffuseColor, 
	vec3 specularColor, 
	float shininess, 
	vec3 normal,
	vec3 viewDir, 
	vec3 lightDir
) {
	vec3 localHalfway = normalize(viewDir + lightDir);
	float specularCos = pow(max(dot(normal, localHalfway), 0.0), shininess);
	float diffuseCos = max(dot(normal, lightDir), 0.0);
	return diffuseColor * diffuseCos 
		+ specularColor * specularCos;
}

vec3 BlurLight(vec2 texCoord, float offset, vec4 directLightAttenuation) {
	vec4 sum = vec4(0);
	int sampleCount = 5;
	for (int i = 0; i < sampleCount; i++) {
		sum += texture(opacityTexture, texCoord + offset * normalize(vec2(noise(vec2(i, i*i)), noise(vec2(i*i*i, i)))));	// rgb = indirect attenuation | a = direct attenuation
	}
	sum += directLightAttenuation * 3;
	sum /= sampleCount + 3;
	return sum.rgb;
}

vec4 calculateColor(vec3 enter, vec3 exit) {	

	vec4 srcLightAttenuation = vec4(0.0);
	vec4 color = vec4(0.0);
	vec4 colorAttenuation = vec4(0.0);	// colorAttenuation = (xyz = Color | w = attenuation)

	vec4 lightModelSpacePos = sceneObject.invModelMatrix * lights[0].position;
	float lightDistance;
	vec3 modelLightDiff;
	if (lightModelSpacePos.w == 0.0) {
		modelLightDiff = lightModelSpacePos.xyz;
		lightDistance = 1.0;
	}
	else {
		lightModelSpacePos /= lightModelSpacePos.w;
		modelLightDiff = lightModelSpacePos.xyz - modelPos * lightModelSpacePos.w;
		lightDistance = length(modelLightDiff);
	}
	vec3 lightDir = normalize(modelLightDiff);
	vec3 modelOpacitySamplePos = intersectPlane(modelPos, lightDir, modelPos + modelHalfwaySlicePlaneDelta * modelHalfwaySlicePlane.normal, modelHalfwaySlicePlane.normal);
	//vec3 modelOpacitySamplePos = modelPos + modelHalfwaySlicePlaneDelta * modelHalfwaySlicePlane.normal;
	vec4 opacitySampleCamSpacePos = viewCamera.viewProjMatrix * sceneObject.modelMatrix * vec4(modelOpacitySamplePos, 1.0);
	opacitySampleCamSpacePos /= opacitySampleCamSpacePos.w;
	vec2 opacitySampleTexCoord = opacitySampleCamSpacePos.xy * 0.5 + vec2(0.5);
	vec4 dstLightAttenuation = texture(opacityTexture, opacitySampleTexCoord);	// dstLightAttenuation = (rgb = indirect attenuation | a = direct attenuation)
	
	if (length(exit - enter) > 0.0) {
		vec4 modelCrossSectionPlanePosition = sceneObject.invModelMatrix * vec4(worldCrossSectionPlane.position, 1);
		modelCrossSectionPlanePosition /= modelCrossSectionPlanePosition.w;
		vec4 modelSlicingPlaneModelNormal = vec4(worldCrossSectionPlane.normal, 0) * sceneObject.modelMatrix;
		if (dot(normalize(modelPos - modelCrossSectionPlanePosition.xyz), modelSlicingPlaneModelNormal.xyz) < 0.0) {
			vec3 noiseOffset = noiseScale * vec3(noise(modelPos.xy), noise(modelPos.yz), noise(modelPos.zx));
			vec4 gradientIntesity = resampleGradientAndIntensity(modelPos + noiseOffset, trilinearInterpolation(modelPos + noiseOffset));	// xyz = gradient | w = intensity
			colorAttenuation = texture(colorAttenuationTransfer, vec2(gradientIntesity.w, length(gradientIntesity.xyz)));	// xyz = Color | w = attenuation
			vec3 viewDir = normalize(enter - exit);
			float cosHalfway = abs(dot(viewDir, modelHalfwaySlicePlane.normal));
			//float cosHalfway = 1.0;
			if (cosHalfway == 0.0) {
				cosHalfway = 1.0;
			}
			vec3 bp = BlinnPhong(colorAttenuation.rgb, specularColor, shininess, normalize(-gradientIntesity.xyz), viewDir, lightDir);
			float g = min(length(gradientIntesity.xyz), 1);	// Using gradient magnitude to interpolate between Blinn-Phong and diffuse color.
			vec3 Cl = g * bp + (1 - g) * colorAttenuation.rgb;// Interpolate between Blinn-Phong and diffuse color, to get color of volume.
			vec3 lightIntensity = lights[0].powerDensity;	// Assuming directional light
			vec3 bluredIndirectLightAttenuation = BlurLight(opacitySampleTexCoord, 0.05 * modelHalfwaySlicePlaneDelta /** cosHalfway*/, dstLightAttenuation);
			// Calculate: L * (alpha + alpha_i) * C_l * d:
			color.rgb = lightIntensity * (
							max(1 - dstLightAttenuation.a, 0) 
							+ max(1 - bluredIndirectLightAttenuation.rgb, vec3(0))
						)
						* Cl * modelHalfwaySlicePlaneDelta / cosHalfway;	
			color.a = max(colorAttenuation.a * modelHalfwaySlicePlaneDelta / cosHalfway, 0.0);
		}
	}
	FragColorAttenuation = colorAttenuation;
	return color;
}

void main() {
	vec4 temp = viewCamera.viewProjMatrix * sceneObject.modelMatrix * vec4(modelPos, 1.0);
	vec2 tex = (temp.xy / temp.w + 1.0) * 0.5;
	vec3 enter = texture(enterTexture, tex).xyz;
	vec3 exit = texture(exitTexture, tex).xyz;
	FragColor = calculateColor(enter, exit);
	//FragColor = vec4(1, 0, 0, 1); // For testing
}