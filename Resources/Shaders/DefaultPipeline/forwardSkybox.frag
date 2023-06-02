#version 420 core

layout (location = 0) out vec4 FragColor;

in VS_OUT {
    vec2 texCoords;
    vec4 rayDir;
} fs_in;

layout (binding = 5) uniform samplerCube skybox;

void main()
{   
    FragColor = vec4(textureLod(skybox, normalize(fs_in.rayDir.xyz), 0).rgb, 0.0);
}