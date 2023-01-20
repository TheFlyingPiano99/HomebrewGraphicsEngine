#version 420 core

in vec2 texCoords;
in vec4 w_rayDir;

layout (location = 0) out vec4 FragColor;


layout (binding = 0) uniform sampler2D frontFaceTexture;
layout (binding = 1) uniform sampler2D transferTexture;
layout (binding = 2) uniform sampler2D backFaceTexture;
layout (binding = 3) uniform sampler3D voxels;
layout (binding = 4) uniform sampler2D lightFaceTexture;

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

uniform vec3 resolution;
uniform float shininess;
uniform vec3 specularColor;
uniform vec3 ambientColor;
uniform bool isBackToFront;
uniform vec3 scale;
uniform float w_delta;
uniform float opacityScale;
uniform bool showNormals;
uniform bool usePBR;
uniform float localShadows;
uniform float gradientBasedLocalIllumination;


struct Light {
	vec3 position;
	vec3 powerDensity;
	mat4 viewProjMatrix;
};
uniform Light light;

struct SceneObject {
	mat4 modelMatrix;
	mat4 invModelMatrix;
};
uniform SceneObject sceneObject;

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


void main()
{   
    vec3 m_enter = texture(frontFaceTexture, texCoords).xyz;
    vec3 m_exit = texture(backFaceTexture, texCoords).xyz;
    float m_distanceInVolume = length(m_exit - m_enter);
    if (0.0001 > m_distanceInVolume) {
        discard;
    }
    vec3 rayDir = normalize(m_exit - m_enter);
    vec3 colorSum = vec3(0.0);
<<<<<<< HEAD
    vec4 oneMinusAttenuation = vec4(1.0);
=======
    float attenuation = 0.0;
>>>>>>> attenuation-blur
    vec4 m_step = sceneObject.invModelMatrix * (normalize(w_rayDir) * w_delta);
    float m_stepSize = length(m_step);
    unsigned int stepCount = unsigned int(m_distanceInVolume / m_stepSize);
    m_enter += resolution / 2.0;  // Move origin from center of volume to corner
    for (int i = 0; i < stepCount; i++) {
		vec3 m_currentPos = m_enter + rayDir * m_stepSize * i;
		vec4 color = transferFunctionFromTexture(m_currentPos);
<<<<<<< HEAD
=======


		color.rgb *= w_delta * opacityScale;									// Correct color
		float absorption = min(max(1.0 - pow(1.0 - color.a, w_delta * opacityScale), 0.0), 1.0);
>>>>>>> attenuation-blur
		vec4 w_currentPos = sceneObject.modelMatrix * vec4(m_currentPos, 1.0);
		w_currentPos /= w_currentPos.w;
		
		vec3 transferGrad = resampleGradientAndDensityFromTransfer(m_currentPos,  trilinearInterpolationFromTransfer(m_currentPos)).xyz;

		// Calculate light contribution:
		vec3 w_lightDir = normalize(light.position - w_currentPos.xyz);
		float w_lightDistance = length(light.position - w_currentPos.xyz);
<<<<<<< HEAD
		vec3 w_halfway = (w_lightDir - normalize(w_rayDir.xyz)) * 0.5;
		vec4 m_halfway = sceneObject.invModelMatrix * vec4(w_halfway, 0.0);

		// Blinn-Phong local illumination:
		float shininess = 10.0;
		float t = min(max(pow(length(transferGrad), 1.0), 0.0), 1.0);
		color.rgb = color.rgb * (1.0 - t) + color.rgb * t * pow(max(dot(normalize(-transferGrad.xyz), normalize(m_halfway.xyz)), 0.0), shininess);
=======
		vec3 w_lightDir = normalize(light.position - w_currentPos.xyz);
		vec3 w_viewDir = normalize(sceneObject.modelMatrix * vec4(-rayDir, 0.0)).xyz;
		vec3 w_halfway = (w_lightDir + w_viewDir) * 0.5;
		vec4 m_halfway = sceneObject.invModelMatrix * vec4(w_halfway, 0.0);
		vec3 transferGrad = resampleGradientAndDensityFromTransfer(m_currentPos, color.a).xyz;
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
>>>>>>> attenuation-blur
		color.rgb *= light.powerDensity / w_lightDistance / w_lightDistance;
		color.rgb *= w_delta * opacityScale;									// Correct color


<<<<<<< HEAD
		color.rgb *= oneMinusAttenuation.rgb;							// Attenuate light in camera direction

		colorSum += color.rgb;												
		oneMinusAttenuation *= pow(1.0 - color.a, w_delta * opacityScale);	// Correct attenuation
    }
    FragColor = vec4(colorSum, 1.0 - oneMinusAttenuation);
=======
		// Calculate opacity of this segment:

		colorSum = colorSum + (1 - attenuation) * color.rgb;
		attenuation = attenuation + absorption * (1 - attenuation);
    }
    FragColor = vec4(colorSum, attenuation);
>>>>>>> attenuation-blur
}