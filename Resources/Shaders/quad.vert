#version 420 core

/*
Simple vertex shader for rendering fullscreen quads
*/

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inTex;

out vec2 texCoords;

void main()
{
    texCoords = inTex;
    gl_Position = vec4(inPos, 0.0, 1.0);
}