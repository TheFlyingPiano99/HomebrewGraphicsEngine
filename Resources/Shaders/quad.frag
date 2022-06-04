#version 420 core

layout(binding=0) uniform sampler2D samplerUnit;

in vec2 texCoords;

layout (location = 0) out vec4 FragColor;

void main()
{
	FragColor = texture(samplerUnit, texCoords);
}