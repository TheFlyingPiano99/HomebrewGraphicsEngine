#version 460 core

in vec2 texCoords;

out vec4 FragColor;

uniform vec4 fillColor;
uniform vec4 borderColor;
uniform ivec2 widthHeight;
uniform ivec4 borderLeftRightTopBottom;

void main()
{   
    vec2 pixelPos = texCoords * widthHeight;
    vec4 color;
    if (
        pixelPos.x < borderLeftRightTopBottom.x
        || float(widthHeight.x) - pixelPos.x < float(borderLeftRightTopBottom.y)
        || pixelPos.y < float(borderLeftRightTopBottom.w)
        || float(widthHeight.y) - pixelPos.y < float(borderLeftRightTopBottom.z)
    ) {
        color = borderColor;
    }
    else {
        color = fillColor;
    }
    FragColor = color;
}