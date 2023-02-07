#version 460 core

layout (location = 0) out vec4 FragColor;

vec2 texCoords;

layout (binding = 0) uniform sampler2D albedoMap;


void main()
{
	vec4 color = texture(albedoMap, texCoords);
	if (color.a < 0.001) {
		discard;
	}
	FragColor = color;
}