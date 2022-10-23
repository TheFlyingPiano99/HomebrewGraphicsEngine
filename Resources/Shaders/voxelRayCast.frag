#version 420 core

in vec2 texCoords;

layout (location = 0) out vec4 FragColor;


layout (binding = 0) uniform sampler2D frontFaceTexture;
layout (binding = 1) uniform sampler2D backFaceTexture;
layout (binding = 2) uniform sampler2D lightFaceTexture;
layout (binding = 3) uniform sampler3D voxels;

uniform vec3 resolution;



void main()
{   
    vec3 m_enter = texture(frontFaceTexture, texCoords).xyz;
    vec3 m_exit = texture(backFaceTexture, texCoords).xyz;
    float m_distanceInVolume = length(m_exit - m_enter);
    if (0.0001 > m_distanceInVolume) {
        discard;
    }
    vec3 rayDir = normalize(m_exit - m_enter);
    vec4 color = vec4(0.0);
    float stepSize = 0.5;
    unsigned int stepCount = unsigned int(m_distanceInVolume / stepSize);
    m_enter -= resolution / 2;  // Move origin from center of volume to corner
    for (int i = 0; i < stepCount; i++) {
        color += texture(voxels, (m_enter + rayDir * i * stepSize) / resolution) * stepSize * 0.1;
    }
    FragColor = color;
}