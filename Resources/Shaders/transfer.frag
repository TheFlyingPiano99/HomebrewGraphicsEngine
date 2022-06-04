#version 420 core

layout(binding=1) uniform sampler2D samplerUnit;

in vec2 texCoords;

layout (location = 0) out vec4 FragColor;

uniform float exposure;
uniform float gamma;

void main()
{
vec4 hdr = texture(samplerUnit, texCoords);
FragColor = vec4(pow(vec3(1.0) - exp(-hdr.xyz * exposure), vec3(1.0 / gamma)), hdr.a);
}