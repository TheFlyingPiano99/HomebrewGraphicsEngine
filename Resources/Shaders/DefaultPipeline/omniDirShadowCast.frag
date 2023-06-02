#version 420 core

in vec4 FragPos;

uniform vec3 casterPosition;
uniform float farPlane;

void main()
{
    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - casterPosition);
    
    // map to [0;1] range by dividing by farPlane
    lightDistance = clamp(lightDistance / farPlane, 0.0, 1.0);
    // write this as modified depth
    if (gl_FragDepth > lightDistance) {
        gl_FragDepth = lightDistance;
    }
}