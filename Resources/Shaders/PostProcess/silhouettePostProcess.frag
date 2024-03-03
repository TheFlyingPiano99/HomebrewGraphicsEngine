#version 460 core
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
    float maxDistance = 0.0;
    for (int u = -1; u <= 1; u += 2) {
        for (int v = -1; v <= 1; v += 2) {
            float _logDepth = texture(screenDepth, fs_in.texCoords + vec2(u,v) / textureSize(screenDepth, 0)).r;
            float _linDepth = linearize_depth(
                                                _logDepth,
                                                0.1,
                                                2000.0
            );
            float d = abs(linDepth - _linDepth);
            if (maxDistance < d) {
                maxDistance = d;
            }
        }
    }
    FragColor = vec4((maxDistance < 0.2)? color : vec3(0), 1);
}