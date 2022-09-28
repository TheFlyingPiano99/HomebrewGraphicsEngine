#version 420 core

layout (binding = 0) uniform sampler2D inputTexture;

in VS_OUT {
vec2 texCoords;
vec4 rayDir;
} fs_in;

layout (location = 0) out vec4 outputColor;

void main()
{
	outputColor = texture(inputTexture, fs_in.texCoords);
	outputColor.r = max(outputColor.r, 0.0);
	outputColor.g = max(outputColor.g, 0.0);
	outputColor.b = max(outputColor.b, 0.0);
	outputColor.a = max(min(outputColor.a, 1.0), 0.0);
}