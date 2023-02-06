#version 460 core

layout (location = 0) out vec4 FragColor;

in VS_OUT {
	vec2 texCoords;
} fs_in;

layout (binding = 0) uniform sampler2D albedoMap;


void main()
{
	if (fs_in.texCoords.x >= 1 || fs_in.texCoords.x <= 0 
	|| fs_in.texCoords.y >= 1 || fs_in.texCoords.y <= 0 ) {
	}
	vec4 color = texture(albedoMap, fs_in.texCoords);
	if (color.a < 0.001) {
		discard;
	}
	FragColor = color;
}