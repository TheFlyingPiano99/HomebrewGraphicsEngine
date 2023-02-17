#version 460 core

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
layout (binding = 2) uniform sampler2D roughnessMetallicAOMap;


void main()
{
	gPosition				= fs_in.worldPos / fs_in.worldPos.w;
	gNormal					= vec4(normalize(fs_in.TBN * (texture(normalMap, fs_in.texCoords).xyz * 2.0 - 1.0)), 0.0);
	gAlbedo					= vec4(texture(albedoMap, fs_in.texCoords).rgb, 1.0);
	gRoughnessMetallicAO	= vec4(texture(roughnessMetallicAOMap, fs_in.texCoords).rgb, 0.0);
}