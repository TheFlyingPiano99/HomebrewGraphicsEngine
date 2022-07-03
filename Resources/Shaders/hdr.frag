#version 420 core
out vec4 FragColor;

in VS_OUT {
    vec2 texCoords;
    vec4 rayDir;
} fs_in;

uniform sampler2D screen;

void main()
{   
    const float gamma = 2.2;
    const float exposure = 1.0;
    vec3 hdrColor = texture(screen, fs_in.texCoords).rgb;
  
    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
  
    FragColor = vec4(mapped, 1.0);
}