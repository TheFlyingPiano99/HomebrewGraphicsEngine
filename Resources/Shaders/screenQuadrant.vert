#version 420 core

layout (location = 0) in vec2 inPos;

out vec2 texCoords;

uniform mat4 modelMatrix;

void main() {
	vec4 ndc_pos = modelMatrix * vec4(inPos.x, inPos.y, 0.0, 1.0);
	ndc_pos /= ndc_pos.w;
	texCoords = (ndc_pos.xy + 1.0) / 2.0;
	gl_Position = ndc_pos;
}
