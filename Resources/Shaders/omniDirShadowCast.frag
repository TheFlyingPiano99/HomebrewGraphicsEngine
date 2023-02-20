#version 420 core

in vec4 FragPos;

uniform vec3 casterPosition;
uniform float farPlane;

void main()
{
    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - casterPosition);
    
    // map to [0;1] range by dividing by farPlane
    lightDistance = lightDistance / farPlane;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
}