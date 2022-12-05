#version 420 core
out vec4 FragColor;

in VS_OUT {
    vec2 texCoords;
    vec4 rayDir;
} fs_in;

layout(binding=0) uniform sampler2D screenColor;


vec3 acesTonemap_simple(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

// Based on http://www.oscars.org/science-technology/sci-tech-projects/aces
vec3 acesTonemap(vec3 color){	
	mat3 m1 = mat3(
        0.59719, 0.07600, 0.02840,
        0.35458, 0.90834, 0.13383,
        0.04823, 0.01566, 0.83777
	);
	mat3 m2 = mat3(
        1.60475, -0.10208, -0.00327,
        -0.53108,  1.10813, -0.07276,
        -0.07367, -0.00605,  1.07602
	);
	vec3 v = m1 * color;    
	vec3 a = v * (v + 0.0245786) - 0.000090537;
	vec3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
	return pow(clamp(m2 * (a / b), 0.0, 1.0), vec3(1.0 / 2.2));	
}

void main()
{   
    vec3 hdrColor = texture(screenColor, fs_in.texCoords).rgb;
 	float brightness = dot(hdrColor, vec3(0.2126, 0.7152, 0.0722));
    // exposure and tone mapping:
    vec3 mapped = acesTonemap(hdrColor);

    const float gamma = 0.7;
    const float exposure = 3.0;

    // exposure correction 
    mapped = vec3(1.0) - exp(-mapped * exposure);
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
  
    FragColor = vec4(mapped, 1.0);
}