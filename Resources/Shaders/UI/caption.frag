#version 420 core

in vec2 texCoords;

out vec4 FragColor;

layout (binding = 0) uniform sampler2D textMap;
uniform vec4 textColor;
uniform vec4 highlightColor;

void main()
{   
    float glyphVal = texture(textMap, texCoords).r;
    FragColor = textColor * glyphVal + highlightColor * (1.0 - glyphVal);
}