#version 420 core

layout (location = 0) out vec4 FragColor;

in vec3 modelPos;

uniform vec4 color;
uniform vec3 modelEye;

void main()
{

	FragColor = color * min(1.0, pow(length(modelPos - modelEye), 3));
}