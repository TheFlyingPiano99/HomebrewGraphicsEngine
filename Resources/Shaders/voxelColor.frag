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
uniform bool showNormals;
uniform bool usePBR;
uniform float localShadows;
uniform float gradientBasedLocalIllumination;


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

vec4 transferFunctionFromTextureNoFilter(vec3 pos) {
	vec4 gradientIntesity = resampleGradientAndDensity(pos, texture(voxels, pos / resolution).r);
	return texture(transferTexture, vec2(gradientIntesity.w, length(gradientIntesity.xyz)));
}


vec4 resampleGradientAndDensityFromTransfer(vec3 position, float intensity)
{
	vec3 normPos = position / resolution;
	vec3 stepSize = 1.0 / resolution;
	vec3 sample0, sample1;
	sample0.x = transferFunctionFromTextureNoFilter(vec3(normPos.x - stepSize.x, normPos.y, normPos.z) * resolution).a;
	sample1.x = transferFunctionFromTextureNoFilter(vec3(normPos.x + stepSize.x, normPos.y, normPos.z) * resolution).a;
	sample0.y = transferFunctionFromTextureNoFilter(vec3(normPos.x, normPos.y - stepSize.y, normPos.z) * resolution).a;
	sample1.y = transferFunctionFromTextureNoFilter(vec3(normPos.x, normPos.y + stepSize.y, normPos.z) * resolution).a;
	sample0.z = transferFunctionFromTextureNoFilter(vec3(normPos.x, normPos.y, normPos.z - stepSize.z) * resolution).a;
	sample1.z = transferFunctionFromTextureNoFilter(vec3(normPos.x, normPos.y, normPos.z + stepSize.z) * resolution).a;
	vec3 scaledPosition = position * resolution - 0.5;
	vec3 fraction = scaledPosition - floor(scaledPosition);
	vec3 correctionPolynomial = (fraction * (fraction - 1.0)) / 2.0;
	intensity += dot((sample0 - intensity * 2.0 + sample1),
	correctionPolynomial);
	return vec4(sample1 - sample0, intensity);
}


const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return num / denom;
}
  
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
} 


void main() {	
	vec3 w_viewDir = normalize(cameraPosition - worldPos);
	float w_delta = opacityScale * length(w_sliceDelta) / abs(dot(normalize(w_sliceDelta), w_viewDir));
	vec3 currentPos = modelPos + resolution * 0.5;
	vec4 ndc_temp = viewProjMatrix * vec4(worldPos, 1);
	ndc_temp /= ndc_temp.w;
	vec2 cameraTexCoords = ndc_temp.xy * 0.5 + 0.5;

	vec4 color = transferFunctionFromTextureNoFilter(currentPos);

	vec3 transferGrad = resampleGradientAndDensityFromTransfer(currentPos, color.a).xyz;
	if (showNormals) {
		transferGrad *= w_delta * 0.1;
		color.rgb = vec3(max(abs(transferGrad.x), 0.0), max(abs(transferGrad.y), 0.0), max(abs(transferGrad.z), 0.0));
		color.a = max(min(1.0 - pow(1.0 - color.a, w_delta), 1.0), 0.0);
		FragColor = color;
		return;
	}

	// Calculate opacity of this segment:
	float w_lightDistance = length(light.position - worldPos);
	vec3 w_lightDir = normalize(light.position - worldPos);
	vec3 w_halfway = (w_lightDir + w_viewDir) * 0.5;
	vec4 m_halfway = sceneObject.invModelMatrix * vec4(w_halfway, 0.0);
	vec3 w_normal = normalize(sceneObject.modelMatrix * vec4(-normalize(transferGrad), 0)).xyz;

	// Blinn-Phong local illumination:
	float shininess = 20.0 * color.a;
	float t = min(max(pow(length(transferGrad) * color.a, 1.0 / (gradientBasedLocalIllumination + 0.0000001)), 0.0), 1.0);
	
	if (usePBR) {
		vec3 albedo = color.rgb;
		float roughness = 0.0001;
		float metallic = 0.9;
		float ao = 0.01;
		vec3 F  = fresnelSchlick(max(dot(w_halfway, w_viewDir), 0.0), mix(vec3(0.04), albedo, metallic));

		color.rgb = (1.0 - t * localShadows) * color.rgb + t * vec3((
							localShadows * (vec3(1.0) - F) * (1.0 - metallic)										// kD 
							* albedo / PI +
																										// Specular: {
							DistributionGGX(w_normal, w_halfway, roughness)									// specular (nominator) {NDF * G * F}
							* GeometrySmith(w_normal, w_viewDir, w_lightDir, roughness) 
							* F						
							/ 4.0 * max(dot(w_normal, w_viewDir), 0.0) * max(dot(w_normal, w_lightDir), 0.0)  + 0.0001)	// specular (denominator) 
																										// }
																									//		Radiance {																								//		}
						* max(dot(w_normal, w_lightDir), 0.0)												// NdotL
						);
	}
	else {
		float kS = color.a * 5.0;
		color.rgb = (1.0 - t * localShadows) * color.rgb + t * (max(dot(w_normal, w_lightDir), 0.0) * color.rgb + kS * pow(max(dot(normalize(-transferGrad.xyz), normalize(m_halfway.xyz)), 0.0), shininess));		
	}
	
	color.rgb *= light.powerDensity / w_lightDistance / w_lightDistance * w_delta;
	vec4 ndc_attenuationCoords = light.viewProjMatrix * vec4(worldPos, 1.0);
	ndc_attenuationCoords /= ndc_attenuationCoords.w;
	vec2 attenuationTexCoords = ndc_attenuationCoords.xy * 0.5 + 0.5;
	vec4 attenuation = texture(attenuationTexture, attenuationTexCoords);

	// Light Attenuation from lights directionlight
	vec3 l = (1.0 - attenuation.a) + (1.0 - attenuation.rgb);
	color.rgb *= vec3(max(l.x, 0.0), max(l.y, 0.0), max(l.z, 0.0));
	color.a = max(min(1.0 - pow(1.0 - color.a, w_delta), 1.0), 0.0);
	FragColor = color;
	//FragColor = vec4(normalize(transferGrad), length(transferGrad));
}