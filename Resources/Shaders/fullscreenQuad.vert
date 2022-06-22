#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBitangent;
layout (location = 4) in vec2 aTex;

out vec2 texCoords;
out vec4 rayDir;

struct Camera {
    mat4 rayDirMatrix;
};
uniform Camera camera;

void main()
{
    texCoords = aTex;
    rayDir = camera.rayDirMatrix * vec4(aPos, 1.0);
    gl_Position = vec4(aPos, 1.0);
}