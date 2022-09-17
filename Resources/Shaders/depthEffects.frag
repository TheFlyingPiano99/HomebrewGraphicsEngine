#version 420 core
out vec4 FragColor;

in VS_OUT {
    vec2 texCoords;
    vec4 rayDir;
} fs_in;

layout(binding=0) uniform sampler2D screenColor;
layout(binding=1) uniform sampler2D screenDepth;

float linearize_depth(float d,float zNear,float zFar)
{
    float z_n = 2.0 * d - 1.0;
    return 2.0 * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));
}

void main()
{   
    vec3 color = texture(screenColor, fs_in.texCoords).rgb;
    float logDepth = texture(screenDepth, fs_in.texCoords).r;
    if (logDepth > 0.999999) {
        FragColor = vec4(color, 1.0);
        return; 
    }
    float linDepth = linearize_depth(
                                        logDepth,
                                        0.1,
                                        2000.0
                                    );
    float t = pow(min(linDepth / 2000.0, 1.0), 0.6);
    float blurT = pow(min(linDepth / 2000.0, 1.0), 0.4);

    vec2 offset = 1.0 / textureSize(screenColor, 0);
    vec4 blurColor = vec4(0.0);
    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            blurColor += texture(screenColor, fs_in.texCoords + offset * vec2(x, y));
        }
    }
    blurColor /= 25.0;

    FragColor = vec4((1 - t) * ((1 - blurT) * color + blurT * blurColor.rgb) + t * vec3(0.4, 0.6, 0.9), 1.0);
}