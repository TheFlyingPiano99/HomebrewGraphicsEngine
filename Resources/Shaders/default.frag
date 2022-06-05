#version 420 core

layout (location = 0) out vec4 FragColor;

// Imports the world position from the Vertex Shader
in vec4 worldPos;
// Imports the normal from the Vertex Shader
in vec4 normal;
// Imports the color from the Vertex Shader
in vec3 color;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;

struct Light {
	vec4 position;
	vec3 powerDensity;
	mat4 viewProjMatrix;
};
uniform Light lights[64];
uniform unsigned int lightCount;

layout (binding = 0) uniform sampler2D colorTexture;
layout (binding = 1) uniform sampler2D specularTexture;
layout (binding = 5) uniform samplerCube skybox;

struct Material {
	vec3 diffuseColor;
	vec3 specularColor;
	vec3 ambientColor;
	float shininess;
	float reflectiveness;
};
uniform Material material;

struct Camera {
	vec3 position;
	mat4 viewProjMatrix;
	mat4 invViewProjMatrix;
};
uniform Camera camera;


vec3 calculateLight(Light light, vec3 fragPos, vec3 normal, vec3 viewDir)
{
	// used in two variables so I calculate it here to not have to do it twice
	vec3 lightDiff = light.position.xyz - fragPos * light.position.w;

	// intensity of light with respect to distance
	float dist = length(lightDiff);
	float attenuation = 1.0 / (dist * dist);

	// diffuse lighting
	vec3 lightDir = normalize(lightDiff);
	float diffuseCos = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diffuseCos * light.powerDensity * attenuation * material.diffuseColor;

	// specular lighting
	vec3 halfway = normalize(lightDir + viewDir);
	float specularCos = pow(max(dot(halfway, normal), 0.0), material.shininess);
	vec3 specular = specularCos * light.powerDensity * attenuation * material.specularColor;

	return diffuse + specular;
}

void main()
{
	vec3 n = normalize(normal.xyz);
	vec3 viewDir = normalize(camera.position - worldPos.xyz);
	vec3 lightSum = vec3(0, 0, 0);
	for (int i = 0; i < lightCount; i++) {
		lightSum += calculateLight(lights[i], worldPos.xyz, n, viewDir);
	}
	lightSum += material.ambientColor;
	vec3 reflectDir = reflect(-viewDir, n);
	float r = material.reflectiveness * (1.0 - max(dot(viewDir, n), 0.0) * 0.8);
	lightSum = max(1.0 - r, 0.0) * lightSum + r * texture(skybox, reflectDir).rgb ;
	FragColor = vec4(lightSum, 1.0f);
	//FragColor = vec4(n, 1);
}