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

/*
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
*/

/*
vec4 calculateColor(vec3 cameraRayStart, vec3 cameraRay) {
	float rayLength = length(cameraRay);
	vec3 currentPos = cameraRayStart + depth * cameraRay;

	//	Calculating depth
	vec4 camSpace = viewProjMatrix * modelMatrix * vec4(currentPos, 1);
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
*/


vec4 transferFunctionFromTexture(float i, float g) {
	return texture(transferTexture, vec2(i, g));
}

vec3 transferGradient(vec3 currentPos, vec4 color) {
	vec3 offset = 1.0 / resolution;
	vec3 centeredPos = ivec3(currentPos);
	vec4 gradientIntesity = resampleGradientAndDensity(centeredPos, trilinearInterpolation(centeredPos + vec3(1,0,0) * offset));
	vec4 cX = transferFunctionFromTexture(length(gradientIntesity.xyz), gradientIntesity.w);

	gradientIntesity = resampleGradientAndDensity(centeredPos, trilinearInterpolation(centeredPos + vec3(0,1,0) * offset));
	vec4 cY = transferFunctionFromTexture(length(gradientIntesity.xyz), gradientIntesity.w);

	gradientIntesity = resampleGradientAndDensity(centeredPos, trilinearInterpolation(centeredPos + vec3(0,0,1) * offset));
	vec4 cZ = transferFunctionFromTexture(length(gradientIntesity.xyz), gradientIntesity.w);

	gradientIntesity = resampleGradientAndDensity(centeredPos, trilinearInterpolation(centeredPos + vec3(-1,0,0) * offset));
	vec4 cNX = transferFunctionFromTexture(length(gradientIntesity.xyz), gradientIntesity.w);

	gradientIntesity = resampleGradientAndDensity(centeredPos, trilinearInterpolation(centeredPos + vec3(0,-1,0) * offset));
	vec4 cNY = transferFunctionFromTexture(length(gradientIntesity.xyz), gradientIntesity.w);

	gradientIntesity = resampleGradientAndDensity(centeredPos, trilinearInterpolation(centeredPos + vec3(0,0,-1) * offset));
	vec4 cNZ = transferFunctionFromTexture(length(gradientIntesity.xyz), gradientIntesity.w);

	float c0 = length(color);
	return (vec3(
				length(cX) - c0, 
				length(cY) - c0, 
				length(cZ) - c0
			)
			+
			 vec3(
				c0 - length(cNX), 
				c0 - length(cNY), 
				c0 - length(cNZ)
			)) * 0.5;
}

void main() {	
	vec3 w_viewDir = normalize(cameraPosition - worldPos);
	float w_delta = opacityScale * length(w_sliceDelta) / abs(dot(normalize(w_sliceDelta), w_viewDir));

	vec3 currentPos = modelPos + resolution * 0.5;
	if (currentPos.x < 1 || currentPos.x > resolution.x - 2
	|| currentPos.y < 1 || currentPos.y > resolution.y  - 2
	|| currentPos.z < 1 || currentPos.z > resolution.z  - 2) {	// Out of volume
		FragColor = vec4(0.0);
		return;
	}
	vec4 gradientIntesity = resampleGradientAndDensity(currentPos, trilinearInterpolation(currentPos));
	vec4 ndc_temp = viewProjMatrix * vec4(worldPos, 1);
	ndc_temp /= ndc_temp.w;
	vec2 cameraTexCoords = ndc_temp.xy * 0.5 + 0.5;
	vec4 prevColor = texture(colorTexture, cameraTexCoords);

	vec4 color = transferFunctionFromTexture(gradientIntesity.w, length(gradientIntesity.xyz));

	vec3 transferGrad = transferGradient(currentPos, color);

	// Calculate opacity of this segment:
	float w_lightDistance = length(light.position - worldPos);
	vec3 w_lightDir = normalize(light.position - worldPos);
	vec3 w_halfway = (w_lightDir + w_viewDir) * 0.5;
	vec4 m_halfway = sceneObject.invModelMatrix * vec4(w_halfway, 0.0);
	
	// Blinn-Phong local illumination:
	float shininess = 3.0;
	float t = pow(length(transferGrad.xyz), 10.0);
	color.rgb += max(1.0 - t, 0.0) * color.rgb + min(t, 1.0) * pow(length(transferGrad), 1.0) * pow(max(dot(normalize(-transferGrad), normalize(m_halfway.xyz)), 0.0), shininess);
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