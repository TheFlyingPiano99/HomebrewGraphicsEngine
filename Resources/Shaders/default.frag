#version 420 core

layout (location = 0) out vec4 FragColor;

in VS_OUT {
	vec4 worldPos;
	vec2 texCoords;
	mat3 TBN;		// Tangent-Bitangent-Normal matrix
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
	Light lights[64];
};

layout (std140, binding = 2) uniform ShadowCaster {
	mat4 lightSpaceMatrix;
};

layout (binding = 0) uniform sampler2D albedoMap;
layout (binding = 1) uniform sampler2D normalMap;
layout (binding = 2) uniform sampler2D metallicMap;
layout (binding = 3) uniform sampler2D roughnessMap;
layout (binding = 4) uniform sampler2D aoMap;
layout (binding = 5) uniform samplerCube skybox;
layout (binding = 6) uniform sampler2D shadowMap;

struct Material {
	vec3 diffuseColor;
	vec3 specularColor;
	vec3 ambientColor;
	float shininess;
	float reflectiveness;
};
uniform Material material;

const float PI = 3.14159265359;

float calculateShadow() {
	vec4 lightPos = lightSpaceMatrix * fs_in.worldPos;
	vec3 projCoords = lightPos.xyz / lightPos.w;
	projCoords = projCoords * 0.5 + 0.5;
	float currentDepth = projCoords.z - 0.0012;
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -2; x <= 2; ++x)
	{
		for(int y = -2; y <= 2; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	return shadow /= 25 * 1.5;
}

vec3 calculateLight(Light light, vec3 fragPos, vec3 diffuseColor, float ks, vec3 normal, vec3 viewDir, float shadow)
{
	// used in two variables so I calculate it here to not have to do it twice
	vec3 lightDiff = light.position.xyz - fragPos * light.position.w;

	// intensity of light with respect to distance
	float dist = length(lightDiff);
	float attenuation = 1.0 / (dist * dist);

	// diffuse lighting
	vec3 lightDir = normalize(lightDiff);
	float diffuseCos = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diffuseCos * light.powerDensity * attenuation * diffuseColor;

	// specular lighting
	vec3 halfway = normalize(lightDir + viewDir);
	float specularCos = pow(max(dot(halfway, normal), 0.0), material.shininess);
	vec3 specular = specularCos * light.powerDensity * attenuation * material.specularColor * ks;

	return (1.0 - shadow) * (diffuse + specular);
}

void main()
{
	vec3 n =  normalize(fs_in.TBN * (texture(normalMap, fs_in.texCoords).xyz * 2.0 - 1.0));
	//vec3 n = normalize(normal.xyz);
	vec3 wp = fs_in.worldPos.xyz / fs_in.worldPos.w;
	vec3 viewDir = normalize(cameraPosition - wp);
	vec3 lightSum = vec3(0, 0, 0);
	vec3 diffColor = texture(albedoMap, fs_in.texCoords).rgb;
	float ks = texture(aoMap, fs_in.texCoords).r;
	float shadow = calculateShadow();
	for (int i = 0; i < lightCount; i++) {
		lightSum += calculateLight(lights[i], wp, diffColor, ks, n, viewDir, shadow);
	}
	lightSum += material.ambientColor;
	vec3 reflectDir = reflect(-viewDir, n);
	float r = (1 - texture(roughnessMap, fs_in.texCoords).r) * ks * material.reflectiveness * (1.0 - max(dot(viewDir, n), 0.0) * 0.8);
	lightSum = max(1.0 - r, 0.0) * lightSum + r * texture(skybox, reflectDir).rgb ;
	FragColor = vec4(lightSum, 1.0f);
}