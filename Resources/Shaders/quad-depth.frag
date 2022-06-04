#version 420 core

layout(binding=0) uniform sampler2D sourceColorSamplerUnit;
layout(binding=1) uniform sampler2D sourceDepthSamplerUnit;
layout(binding=6) uniform sampler2D tagetDepthSamplerUnit;

in vec2 texCoords;

layout (location = 0) out vec4 FragColor;

void main()
{
	float exposure = 1.0;
	float gamma = 1.0;
	float sourceDepth = texture(sourceDepthSamplerUnit, texCoords).x;
	if (sourceDepth < texture(tagetDepthSamplerUnit, texCoords).x) {
		gl_FragDepth = sourceDepth;
		/*
		vec4 hdr = texture(sourceColorSamplerUnit, texCoords);
		// HDR Tone mapping
		vec3 result = vec3(1.0) - exp(-hdr.rgb * exposure);
		// GAMMA CORRECTION
		result = pow(result, vec3(1.0 / gamma));
		FragColor = vec4(result, hdr.a);*/
		FragColor = texture(sourceColorSamplerUnit, texCoords);
	}
}