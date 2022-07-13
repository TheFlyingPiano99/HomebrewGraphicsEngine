#version 330 core
in vec2 texCoords;
out vec4 color;

layout (binding = 0) uniform sampler2D glyph;

void main()
{    
    color = vec4(texture(glyph, texCoords).r, 0.0, 0.0, 1.0);
}