#version 420 core

layout (location = 0) out vec4 FragColor;

in vec2 texCoords;

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

uniform struct Light {			// aligned size: 32 bytes
	vec4 direction;		
	vec3 powerDensity;
	mat4 lightSpaceMatrix;
} light;

layout (binding = 0) uniform sampler2D gPosition;
layout (binding = 1) uniform sampler2D gAlbedo;
layout (binding = 2) uniform sampler2D gNormal;
layout (binding = 3) uniform sampler2D gRoughnessMetallicAO;
//...
layout (binding = 5) uniform samplerCube environmentMap;
layout (binding = 6) uniform samplerCube irradianceMap;
layout (binding = 7) uniform samplerCube prefilterMap;
layout (binding = 8) uniform sampler2D brdfLUT;


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

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

void main()
{
	vec4 wp4 = texture(gPosition, texCoords);
	if (wp4.w < 0.0001) {
		discard;
	}
	vec3 n =  normalize(texture(gNormal, texCoords).xyz);
	vec4 albedo4 = texture(gAlbedo, texCoords);
	if (albedo4.w < 0.0001) {		// w = 0 means no shading
		FragColor = vec4(albedo4.rgb, 1.0);	// w = 1 prevents adding same fragment color multiple times
		return;
	}
	vec3 viewDir = normalize(cameraPosition - wp4.xyz);
	vec3 reflectedDir = reflect(-viewDir, n);
	vec3 roughnessMetallicAO = texture(gRoughnessMetallicAO, texCoords).rgb;

	vec3 F0 = mix(vec3(0.04), albedo4.rgb, roughnessMetallicAO.y);
	vec3 F = fresnelSchlickRoughness(max(dot(n, viewDir), 0.0), F0, roughnessMetallicAO.x);
	vec3 kS = F;
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - roughnessMetallicAO.y; 

	vec3 irradiance = texture(irradianceMap, n).rgb;
	vec3 diffuse    = irradiance * albedo4.rgb;
  
	const float MAX_REFLECTION_LOD = 4.0;
	vec3 prefilteredColor = textureLod(prefilterMap, reflectedDir,  roughnessMetallicAO.x * MAX_REFLECTION_LOD).rgb;
	vec2 envBRDF  = texture(brdfLUT, vec2(max(dot(n, viewDir), 0.0), roughnessMetallicAO.x)).rg;
	vec3 specular = prefilteredColor * (kS * envBRDF.x + envBRDF.y);

	vec3 ambient = (kD * diffuse + specular) * roughnessMetallicAO.z;
	FragColor = vec4(ambient, 1.0);
}