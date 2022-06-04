#version 420 core
// Positions/Coordinates
layout (location = 0) in vec4 aPos;
// Normals (not necessarily normalized)
layout (location = 1) in vec3 aNormal;
// Colors
layout (location = 2) in vec3 aColor;
// Texture Coordinates
layout (location = 3) in vec2 aTex;

out vec2 texCoords;
out vec4 rayDir;

struct Camera {
    mat4 rayDirMatrix;
};
uniform Camera camera;

void main()
{
    texCoords = aTex;
    rayDir = camera.rayDirMatrix * aPos;
    gl_Position = aPos;
}