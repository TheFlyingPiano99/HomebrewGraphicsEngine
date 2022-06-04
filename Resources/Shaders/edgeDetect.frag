#version 420 core
out vec4 FragColor;

in vec2 texCoords;
in vec4 rayDir;

uniform sampler2D screen;

void main()
{   
    vec3 color0 = texture(screen, texCoords).xyz;
    vec3 color1 = texture(screen, texCoords + vec2(0.002, 0.0)).xyz;
    vec3 color2 = texture(screen, texCoords + vec2(0.0, 0.002)).xyz;
    FragColor = vec4(color0 - max(length(color0 - color1), length(color0 - color2)), 1);
}