#version 420 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragOpacity;
in vec2 texCoords;

layout (binding = 0) uniform sampler3D voxels;
layout (binding = 1) uniform sampler2D colorAttenuationTransfer;
layout (binding = 2) uniform sampler2D enterTexture;
layout (binding = 3) uniform sampler2D exitTexture;
layout (binding = 4) uniform sampler2D opacityTexture;
layout (binding = 5) uniform samplerCube skybox;
layout (binding = 6) uniform sampler2D targetDepthTexture;

layout (binding = 7) uniform sampler2D lightExitTexture0;
layout (binding = 8) uniform sampler2D lightExitTexture1;
layout (binding = 9) uniform sampler2D lightExitTexture2;
layout (binding = 10) uniform sampler2D lightExitTexture3;
layout (binding = 11) uniform sampler2D lightExitTexture4;
layout (binding = 12) uniform sampler2D lightExitTexture5;
layout (binding = 13) uniform sampler2D lightExitTexture6;

uniform vec3 resolution;
uniform mat4 modelMatrix;
uniform mat4 invModelMatrix;

uniform float shininess;
uniform vec3 specularColor;
uniform vec3 ambientColor;

uniform unsigned int shadowSamples;
uniform bool opacityMode;
uniform float depth;
uniform unsigned int stepCount;

struct Camera {
	mat4 viewProjMatrix;
};
uniform Camera camera;

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


vec3 calculateLightLevel(int lightIdx, vec3 modelPos, vec3 diffuseColor, vec3 specularColor, float shininess, Light light, vec3 gradient, vec3 modelEyePos) {
	vec4 lightCameraSpaceCoord = light.viewProjMatrix * modelMatrix * vec4(modelPos, 1);
	vec2 tex = (lightCameraSpaceCoord.xy / lightCameraSpaceCoord.w + vec2(1, 1)) / vec2(2.0, 2.0);
	vec3 lightBeamEndPos;
	switch(lightIdx) {
		case 0: lightBeamEndPos	= texture(lightExitTexture0, tex).xyz; break;
		case 1: lightBeamEndPos	= texture(lightExitTexture1, tex).xyz; break;
		case 2: lightBeamEndPos	= texture(lightExitTexture2, tex).xyz; break;	
		case 3: lightBeamEndPos	= texture(lightExitTexture3, tex).xyz; break;
		case 4: lightBeamEndPos	= texture(lightExitTexture4, tex).xyz; break;
		case 5: lightBeamEndPos	= texture(lightExitTexture5, tex).xyz; break;
		case 6: lightBeamEndPos	= texture(lightExitTexture6, tex).xyz; break;
	}
	float distanceToBeamEndPos = length(lightBeamEndPos - modelPos);

	float delta = distanceToBeamEndPos / float(shadowSamples + 1) + 5.0 * noise(texCoords);
	float distanceTravelled = delta;

	vec4 lightSourceModelPos = invModelMatrix * light.position;
	if (lightSourceModelPos.w > 0.000000001) {
		lightSourceModelPos /= lightSourceModelPos.w;
	}
	vec3 toLightSource = lightSourceModelPos.xyz - modelPos * lightSourceModelPos.w;
	vec3 lightDir = normalize(toLightSource);
	vec3 samplePos = modelPos + lightDir * delta;
	float disanceToLightSource = length(toLightSource);
	if (lightSourceModelPos.w == 0.0) {
		disanceToLightSource = 1.0;
	}
	float opacity = 1.0;
	int i = 0;
	while(distanceTravelled < distanceToBeamEndPos) {
		vec4 gradientIntesity = resampleGradientAndDensity(samplePos, texture(voxels, samplePos).a);
		opacity *= pow(1 - texture(colorAttenuationTransfer, vec2(gradientIntesity.w, length(gradientIntesity.xyz))).a, delta);
		samplePos += lightDir * delta;
		distanceTravelled += delta;
		i++;
	}
	vec3 normal = normalize(-gradient);
	vec3 viewDir = normalize(modelEyePos - modelPos);
	vec3 reflectionDir = reflect(-viewDir, normal);
	vec3 halfway = normalize(viewDir + lightDir);
	float specularCos = pow(max(dot(normal, halfway), 0.0), shininess);
	float diffuseCos = max(dot(normal, lightDir), 0.0);
	vec3 intensity = opacity * light.powerDensity / disanceToLightSource / disanceToLightSource;
	
	return diffuseColor * intensity * diffuseCos 
		+ specularColor * intensity * specularCos * length(gradient);
}

vec4 calculateColor(vec3 cameraRayStart, vec3 cameraRay) {
	float rayLength = length(cameraRay);
	vec3 currentPos = cameraRayStart + depth * cameraRay;

	//	Calculating depth
	vec4 camSpace = camera.viewProjMatrix * modelMatrix * vec4(currentPos, 1);
	camSpace = camSpace / camSpace.w;
	camSpace.z = 0.5 * (camSpace.z + 1);

	float targetDepth = texture(targetDepthTexture, texCoords).x;
	if (0.000001 < rayLength && camSpace.z < targetDepth) {
		vec4 color = vec4(0.0);
		float delta = rayLength / float(stepCount);
		float opacity = texture(opacityTexture, texCoords).x;
		vec4 gradientIntesity = resampleGradientAndDensity(currentPos, trilinearInterpolation(currentPos));
		vec4 colorAttenuation = texture(colorAttenuationTransfer, vec2(gradientIntesity.w, length(gradientIntesity.xyz)));
		vec3 lightLevel = vec3(0.0);
		lightLevel += calculateLightLevel(0, currentPos, colorAttenuation.rgb, specularColor, shininess, lights[0], gradientIntesity.xyz, cameraRayStart);
		color.rgb = delta * opacity * lightLevel;
		color.a = colorAttenuation.a * delta;
		gl_FragDepth = camSpace.z;
		opacity *= pow(1 - colorAttenuation.a, delta);
		FragOpacity = vec4(opacity, 0, 0, 1);
		return color;
	}
	else {
		FragOpacity = vec4(0, 0, 0, 1);
		return vec4(0.0);
	}
}

void main() {	
	vec3 start = texture(enterTexture, texCoords).xyz;
	vec3 ray = texture(exitTexture, texCoords).xyz - start;
	FragColor = calculateColor(start, ray);
}