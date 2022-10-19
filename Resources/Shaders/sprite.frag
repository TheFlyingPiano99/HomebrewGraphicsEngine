#version 460 core

layout (location = 0) out vec4 FragColor;

in VS_OUT {
	vec4 worldPos;
	vec2 texCoords;
	mat3 TBN;		// Tangent-Bitangent-Normal matrix
} fs_in;

layout (binding = 0) uniform sampler2D albedoMap;


void main()
{
	vec4 color = texture(albedoMap, fs_in.texCoords);
	FragColor = color;
}