#version 420 core

layout (binding = 0) uniform sampler2D bloomTexture;
layout (binding = 1) uniform sampler2D hdrTexture;

in vec2 texCoords;
layout (location = 0) out vec3 combined;
uniform float mixBloom;

void main()
{
	// If brightness of input color is greater then the treshold than the color is outputed
	vec3 bloom = texture(bloomTexture, texCoords).rgb;
	vec3 hdrColor = texture(hdrTexture, texCoords).rgb;
	combined = hdrColor + mixBloom * bloom;
}