#version 420 core
/*
*   Vertex shader for rendering simple 2D sprites in screen-space
*   No depth information is considered
*/ 

layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 texCoords;

void main()
{
    gl_Position = vec4(vertex.xy, 0.0, 1.0);
    texCoords = vertex.zw;
}