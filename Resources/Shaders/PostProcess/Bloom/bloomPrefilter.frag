#version 420 core

layout (binding = 1) uniform sampler2D hdrTexture;

in vec2 texCoords;
layout (location = 0) out vec3 filtered;

uniform float treshold;

void main()
{
	// If brightness of input color is greater then the treshold than the color is outputed
	vec3 hdrColor = texture(hdrTexture, texCoords).rgb;
	filtered = (dot(hdrColor, vec3(0.2126, 0.7152, 0.0722)) > treshold)? hdrColor : vec3(0.0);	
}