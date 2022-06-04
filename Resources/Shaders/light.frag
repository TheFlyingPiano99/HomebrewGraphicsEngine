#version 420 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform vec3 lightColor;

void main()
{
	FragColor = vec4(vec3(10000, 10000, 10000), 1.0f);
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));	// Convert to grayscale.
	if (brightness > 1.0) {
		BrightColor = vec4(FragColor.rgb, 1.0);
	}
	else {
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}