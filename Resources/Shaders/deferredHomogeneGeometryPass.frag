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

struct Material {
	vec3 albedo;
	float roughness;
	float metallic;
	float ao;
};

uniform Material material;	// not used now!

void main()
{
	gPosition				= fs_in.worldPos / fs_in.worldPos.w;
	gNormal					= vec4(normalize(fs_in.TBN * vec3(0.0, 0.0, 1.0)), 0.0);
	gAlbedo					= vec4(material.albedo, 1.0);		// w = 1 tells that the material shoud be shaded
	gRoughnessMetallicAO	= vec4(material.roughness, material.metallic, material.ao, 0.0);
}