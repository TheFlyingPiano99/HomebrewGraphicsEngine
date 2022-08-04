#version 420 core
in vec2 texCoords;
out vec4 roughnessMetallicAO;

layout (binding = 0) uniform sampler2D roughnessMap;
layout (binding = 1) uniform sampler2D metallicMap;
layout (binding = 2) uniform sampler2D aoMap;

void main()
{    
    roughnessMetallicAO = vec4(
        texture(roughnessMap, texCoords).r,
        texture(metallicMap, texCoords).r,
        texture(aoMap, texCoords).r,
        1.0
    );
}