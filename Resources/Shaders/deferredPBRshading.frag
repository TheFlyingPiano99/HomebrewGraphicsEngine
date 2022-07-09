#version 420 core

layout (location = 0) out vec4 FragColor;

in VS_OUT {
    vec2 texCoords;
    vec4 rayDir;
} fs_in;

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

struct Light {			// aligned size: 32 bytes
	vec4 position;		
	vec3 powerDensity;
};

layout (std140, binding = 1) uniform Lights {
	unsigned int lightCount;
	Light lights[1024];
};

layout (std140, binding = 2) uniform ShadowCaster {
	mat4 lightSpaceMatrix;
};

layout (binding = 0) uniform sampler2D gPosition;
layout (binding = 1) uniform sampler2D gNormal;
layout (binding = 2) uniform sampler2D gAlbedo;
layout (binding = 3) uniform sampler2D gRoughnessMetallicAO;
//...
layout (binding = 5) uniform samplerCube skybox;
layout (binding = 6) uniform sampler2D shadowMap;


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

float calculateShadow(vec3 wp) {
	vec4 lightPos = lightSpaceMatrix * vec4(wp, 1.0);
	vec3 projCoords = lightPos.xyz / lightPos.w;
	projCoords = projCoords * 0.5 + 0.5;
	float currentDepth = projCoords.z - 0.0008;
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -2; x <= 2; ++x)
	{
		for(int y = -2; y <= 2; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += (currentDepth < 1.0 && currentDepth > pcfDepth) ? 1.0 : 0.0;        
		}    
	}
	return shadow /= 25 * 1.5;
}

void main()
{
	vec4 wp4 = texture(gPosition, fs_in.texCoords);
	if (wp4.w < 0.0001) {
		discard;
	}
	vec3 wp = wp4.xyz;
	vec3 n =  texture(gNormal, fs_in.texCoords).xyz;
	vec4 albedo4 = texture(gAlbedo, fs_in.texCoords);
	if (albedo4.w < 0.0001) {
		FragColor = vec4(albedo4.rgb, 1.0);
		return;
	}
	vec3 albedo = albedo4.rgb;
	float roughness = texture(gRoughnessMetallicAO, fs_in.texCoords).r;
	float metallic = texture(gRoughnessMetallicAO, fs_in.texCoords).g;
	float ao = texture(gRoughnessMetallicAO, fs_in.texCoords).b;
	vec3 viewDir = normalize(cameraPosition - wp);
	float shadow = calculateShadow(wp);
	vec3 Lo = vec3(0, 0, 0);
	vec3 F0 = vec3(0.04); 
	for (int i = 0; i < lightCount; i++) {
		vec3 lightDiff = lights[i].position.xyz - wp * lights[i].position.w;
		float lightDistance = length(lightDiff);
		vec3 lightDir = lightDiff / lightDistance;
		vec3 halfway = normalize(viewDir + lightDir);
		float attenuation = 1.0 / (lightDistance * lightDistance);
        vec3 radiance     = lights[i].powerDensity * attenuation * (1.0 - shadow);
		F0      = mix(F0, albedo, metallic);
		vec3 F  = fresnelSchlick(max(dot(halfway, viewDir), 0.0), F0);
		float NDF = DistributionGGX(n, halfway, roughness);       
		float G   = GeometrySmith(n, viewDir, lightDir, roughness);
		vec3 numerator    = NDF * G * F;
		float denominator = 4.0 * max(dot(n, viewDir), 0.0) * max(dot(n, lightDir), 0.0)  + 0.0001;
		vec3 specular     = numerator / denominator; 
		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;
	    float NdotL = max(dot(n, lightDir), 0.0);        
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;
	   
    FragColor = vec4(color, 1.0);
}