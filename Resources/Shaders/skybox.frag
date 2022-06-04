#version 420 core
out vec4 FragColor;

in vec2 texCoords;
in vec4 rayDir;

uniform samplerCube skybox;

void main()
{   
    FragColor = texture(skybox, normalize(rayDir.xyz));
}