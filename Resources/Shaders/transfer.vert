#version 420 core

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inTexCoords;

out vec2 texCoords;

uniform mat4 modelMatrix;

void main() {
	gl_Position = modelMatrix * vec4(inPos.x, inPos.y, 0.0, 1.0);
	texCoords = inTexCoords;
}
