#version 420 core
/*
For creating caption textures
*/

in vec2 texCoords;
out float FragColor;

layout (binding = 0) uniform sampler2D glyph;

void main()
{    
    FragColor = texture(glyph, texCoords).r;
}