#version 420 core

// This shader performs upsampling on a texture,
// as taken from Call Of Duty method, presented at ACM Siggraph 2014.

// Remember to add bilinear minification filter for this texture!
// Remember to use a floating-point texture format (for HDR)!
// Remember to use edge clamping for this texture!
layout (binding = 0) uniform sampler2D srcTexture;

in vec2 texCoords;
layout (location = 0) out vec3 upsample;
uniform int mipLevel;
uniform float falloff;

void main()
{
    vec2 srcTexelSize = 1.0 / textureSize(srcTexture, mipLevel);
    float x = srcTexelSize.x;
    float y = srcTexelSize.y;

    // Take 9 samples around current texel:
    // a - b - c
    // d - e - f
    // g - h - i
    // === ('e' is the current texel) ===
    vec3 a = textureLod(srcTexture, vec2(texCoords.x - x, texCoords.y + y), mipLevel).rgb;
    vec3 b = textureLod(srcTexture, vec2(texCoords.x,     texCoords.y + y), mipLevel).rgb;
    vec3 c = textureLod(srcTexture, vec2(texCoords.x + x, texCoords.y + y), mipLevel).rgb;

    vec3 d = textureLod(srcTexture, vec2(texCoords.x - x, texCoords.y), mipLevel).rgb;
    vec3 e = textureLod(srcTexture, vec2(texCoords.x,     texCoords.y), mipLevel).rgb;
    vec3 f = textureLod(srcTexture, vec2(texCoords.x + x, texCoords.y), mipLevel).rgb;

    vec3 g = textureLod(srcTexture, vec2(texCoords.x - x, texCoords.y - y), mipLevel).rgb;
    vec3 h = textureLod(srcTexture, vec2(texCoords.x,     texCoords.y - y), mipLevel).rgb;
    vec3 i = textureLod(srcTexture, vec2(texCoords.x + x, texCoords.y - y), mipLevel).rgb;

    // Apply weighted distribution, by using a 3x3 tent filter:
    //  1   | 1 2 1 |
    // -- * | 2 4 2 |
    // 16   | 1 2 1 |
    upsample = e * 4.0;
    upsample += (b + d + f + h) * 2.0;
    upsample += (a + c + g + i);
    upsample *= 1.0 / 16.0 * falloff;
}