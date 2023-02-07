#version 420 core
/*
*   Vertex shader for rendering simple 2D sprites in screen-space
*   No depth information is considered
*/ 

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inTex;

out vec2 texCoords;

uniform mat4 transform;

void main()
{
    texCoords = inTex;
    gl_Position = transform * vec4(inPos.xy, 0.0, 1.0);
}