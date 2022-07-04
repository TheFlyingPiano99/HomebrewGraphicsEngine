#version 420 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gRoughnessMetallicAO;

in VS_OUT {
    vec2 texCoords;
    vec4 rayDir;
} fs_in;

layout (binding = 5) uniform samplerCube skybox;

void main()
{   
    gPosition = vec4(0, 0, 0, 1);
    gAlbedo = vec4(texture(skybox, normalize(fs_in.rayDir.xyz)).rgb, 0.0);
}