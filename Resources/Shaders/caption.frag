#version 330 core
in vec2 texCoords;
out vec4 color;

layout (binding = 0) uniform sampler2D textMap;
uniform vec4 textColor;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(textMap, texCoords).r);
    color = textColor * sampled;
}