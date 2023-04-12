#version 420 core
in vec2 texCoords;
out vec4 roughnessMetallicAO;

layout (binding = 0) uniform sampler2D roughnessMap;
layout (binding = 1) uniform sampler2D metallicMap;
layout (binding = 2) uniform sampler2D aoMap;

void main()
{    
    roughnessMetallicAO = vec4(
        clamp(texture(roughnessMap, texCoords).r, 0, 255),
        clamp(texture(metallicMap, texCoords).r, 0, 255),
        clamp(texture(aoMap, texCoords).r, 0, 255),
        1.0
    );
}