#version 420 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gRoughnessMetallicAO;

in VS_OUT {
	vec4 worldPos;
	vec2 texCoords;
	mat3 TBN;		// Tangent-Bitangent-Normal matrix
} fs_in;

layout (binding = 0) uniform sampler2D albedoMap;
layout (binding = 1) uniform sampler2D normalMap;
layout (binding = 2) uniform sampler2D roughnessMap;
layout (binding = 3) uniform sampler2D metallicMap;
layout (binding = 4) uniform sampler2D aoMap;
layout (binding = 5) uniform samplerCube skybox;
layout (binding = 6) uniform sampler2D shadowMap;

struct Material {
	vec3 albedo;
	float roughness;
	float metallic;
	float ao;
};
uniform Material material;	// not used now!

void main()
{
	vec4 wp = fs_in.worldPos / fs_in.worldPos.w;
	vec3 n =  normalize(fs_in.TBN * (texture(normalMap, fs_in.texCoords).xyz * 2.0 - 1.0));
	vec3 albedo = texture(albedoMap, fs_in.texCoords).rgb;
	float roughness = texture(roughnessMap, fs_in.texCoords).r;
	float metallic = texture(metallicMap, fs_in.texCoords).r;
	float ao = texture(aoMap, fs_in.texCoords).r;
	
	gPosition				= wp;
	gNormal					= vec4(n, 0.0);
	gAlbedo					= vec4(albedo, 1.0);
	gRoughnessMetallicAO	= vec4(roughness, metallic, ao, 0.0);
}