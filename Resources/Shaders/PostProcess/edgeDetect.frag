#version 420 core
out vec4 FragColor;

in VS_OUT {
    vec2 texCoords;
    vec4 rayDir;
} fs_in;

uniform sampler2D screen;

void main()
{   
    vec3 color0 = texture(screen, fs_in.texCoords).xyz;
    vec3 color1 = texture(screen, fs_in.texCoords + vec2(0.001, 0.0)).xyz;
    vec3 color2 = texture(screen, fs_in.texCoords + vec2(0.0, 0.001)).xyz;
    FragColor = vec4(color0 - max(length(color0 - color1), length(color0 - color2)), 1);
}