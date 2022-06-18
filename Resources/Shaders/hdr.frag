#version 420 core
out vec4 FragColor;

in vec2 texCoords;
in vec4 rayDir;

uniform sampler2D screen;

void main()
{   
    const float gamma = 2.2;
    const float exposure = 1.2;
    vec3 hdrColor = texture(screen, texCoords).rgb;
  
    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
  
    FragColor = vec4(mapped, 1.0);}