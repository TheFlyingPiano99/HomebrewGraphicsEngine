#version 420 core

/*
	This shader program is for clearing the scene behind objects that will get shaded in the deferred lighting pass.
	It is cesessary, because the lighting gets applied additively.
	Otherwise dark object would appear to be transparent.
*/

layout (location = 0) out vec4 FragColor;

in vec2 texCoords;

layout (binding = 0) uniform sampler2D gPosition;
layout (binding = 2) uniform sampler2D gAlbedo;

void main()
{
	vec4 wp4 = texture(gPosition, texCoords);
	if (wp4.w < 0.0001) {
		discard;
	}
	vec3 wp = wp4.xyz;
	vec4 albedo4 = texture(gAlbedo, texCoords);
	if (albedo4.w < 0.0001) {		// w = 0 means no shading
		FragColor = vec4(0.0);	// w = 1 prevents adding same fragment color multiple times
		return;
	}
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}