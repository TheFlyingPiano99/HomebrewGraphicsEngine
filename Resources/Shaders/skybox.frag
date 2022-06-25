#version 420 core
out vec4 FragColor;

in VS_OUT {
    vec2 texCoords;
    vec4 rayDir;
} fs_in;

layout (binding = 5) uniform samplerCube skybox;

void main()
{   
    FragColor = texture(skybox, normalize(fs_in.rayDir.xyz));
}