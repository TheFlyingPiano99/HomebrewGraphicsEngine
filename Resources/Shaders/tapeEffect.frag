#version 420 core
out vec4 FragColor;

in VS_OUT {
    vec2 texCoords;
    vec4 rayDir;
} fs_in;

layout(binding=0) uniform sampler2D screenColor;

uniform float time;

void main()
{   
    vec2 centeredCoord = fs_in.texCoords * 2.0 - 1.0;
    vec2 size = textureSize(screenColor, 0);
    float t = pow(min(dot(centeredCoord, centeredCoord) * 0.5, 1.0), 3.0);
    vec2 tex = fs_in.texCoords + normalize(centeredCoord) / size * 10.0 * t;
    vec2 texNoise = vec2(0.0);
    int y = int(tex.y * size.y * 0.5);
    vec3 scanLine = vec3(0.0);
    if ((y % 2) == 0) {
        scanLine = vec3(0, 0.05, 0.008);
    }
    texNoise.x = sin(tex.y * time * 10) / size.x * 0.8;
    if ((int(tex.x) % int(time)) == 0 && (int(tex.y) % int(time)) == 0 && (1000 % int(1000.0 * sin(y * time * 5) * time * tex.x) < 5)) {
        float scanT = sin(tex.x * tex.y * time + 10);
        scanLine = scanT * vec3(0.5, 0.5, 0.5) * 0.5;
    }
    float chromaticAberationAmount = (0.1 + 0.9 * t) * 50.0;
    vec3 color = vec3(
                    texture(screenColor, tex - chromaticAberationAmount * vec2(1.0 / size.x, 0.0) + texNoise).r, 
                    texture(screenColor, tex + texNoise).g, 
                    texture(screenColor, tex + chromaticAberationAmount * vec2(1.0 / size.x, 0.0) + texNoise).b
                    );
    vec3 processed = color * (0.6 + 0.4 * (1.0 - t)) + scanLine;
    FragColor = vec4(processed, 1.0);
}