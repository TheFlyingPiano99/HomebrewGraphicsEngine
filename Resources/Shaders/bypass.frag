#version 420 core

layout (binding = 0) uniform sampler2D inputTexture;

vec2 texCoords;

layout (location = 0) out vec4 outputColor;

void main()
{
	outputColor = texture(inputTexture, texCoords);
	outputColor.r = max(outputColor.r, 0.0);
	outputColor.g = max(outputColor.g, 0.0);
	outputColor.b = max(outputColor.b, 0.0);
	outputColor.a = max(min(outputColor.a, 1.0), 0.0);
}