#version 420 core

/*
	Half angle method
*/

in vec3 modelPos;

layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform sampler2D colorAttenuationTexture;
layout (binding = 4) uniform sampler2D opacityTexture;

uniform vec3 resolution;

struct SceneObject {
	mat4 modelMatrix;
	mat4 invModelMatrix;
};
uniform SceneObject sceneObject;

float modelHalfwaySlicePlaneDelta;

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

vec4 calculateAttenuation(vec2 lightTex) {	
	vec4 temp = viewCamera.viewProjMatrix * sceneObject.modelMatrix * vec4(modelPos, 1.0);
	vec2 viewTex = (temp.xy / temp.w + 1.0) * 0.5;	
	vec4 colorAttenuation = texture(colorAttenuationTexture, viewTex);	// colorAttenuation = (xyz = Color | w = attenuation)
	//vec4 previousOpacity = texture(opacityTexture, lightTex);
	/*
	float cosHalfway = abs(dot(viewDir, modelHalfwaySlicePlane.normal));
	//float cosHalfway = 1.0;
	if (cosHalfway == 0.0) {
		cosHalfway = 1.0;
	}
	*/
	vec3 indirectAttenuation = colorAttenuation.a * normalize(1 - colorAttenuation.rgb);	// Heuristic calculation to get translucent volume.
	return max(vec4(indirectAttenuation, colorAttenuation.a) * modelHalfwaySlicePlaneDelta /*/ cosHalfway*/, vec4(0.0));	// Calculate indirect (rgb) and direct (a) light attenuation
}

void main() {
	vec4 temp = lightCamera.viewProjMatrix * sceneObject.modelMatrix * vec4(modelPos, 1.0);
	vec2 lightTex = (temp.xy / temp.w + 1.0) * 0.5;
	FragColor = calculateAttenuation(lightTex);
}