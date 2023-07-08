#version 460 core

in vec2 texCoords;


layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform sampler2D sprite;

void main()
{
	FragColor = texture(sprite, texCoords);
}