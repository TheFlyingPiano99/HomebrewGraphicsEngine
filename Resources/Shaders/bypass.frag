#version 420 core

layout (binding = 0) uniform sampler2D inputTexture;

in VS_OUT {
vec2 texCoords;
vec4 rayDir;
} fs_in;

layout (location = 0) out vec3 outputColor;

void main()
{
	outputColor = texture(inputTexture, fs_in.texCoords).rgb;
}