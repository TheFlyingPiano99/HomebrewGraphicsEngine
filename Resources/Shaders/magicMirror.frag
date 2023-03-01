#version 420 core
out vec4 FragColor;

in vec2 texCoords;

uniform float mirrorDistance;
uniform float mirrorShininess;
uniform float mirrorDepth;
uniform float mirrorCurvatureRadius;
uniform vec3 mirrorSize;
uniform vec3 lightDir;
uniform float lightIntensity;
uniform int resolution;
uniform float traceAreaScale;

vec4 normHeight(vec2 uv) {
    float s0 = 0.2;
    float s1 = 0.6;
    float c = 3; 
    float h = sqrt(pow(mirrorCurvatureRadius, 2) - pow(2 * uv.x - 1, 2)) - sqrt(pow(mirrorCurvatureRadius, 2) - 1);
    vec3 tanU = vec3(1, 0, (2 - 4 * uv.x) / sqrt(pow(mirrorCurvatureRadius, 2) - pow(2 * uv.x - 1, 2)));
    vec3 tanV;
    if (uv.y < s0 || uv.y > s1) {
        tanV = vec3(0, 1, 0);
    }
    else if (uv.y >= s0 && uv.y <= s1) {
        h += mirrorDepth * 0.5 * (tanh(
                                c * (
                                    2 * (uv.y - s0) / (s1 - s0) - 1.0
                                )
                            ) / tanh(c) + 1.0
                  );
        tanV = vec3(0, 1, 
                mirrorDepth * c / (tanh(c) * (s1 - s0)) * (
                    1.0 - pow(
                        tanh(
                            c * (
                                2 * (uv.y - s0) / (s1 - s0) - 1
                            )
                        ), 2
                    )
                )
        );
    }
    if (uv.y > s1) {
        h += mirrorDepth;
    }

    return vec4(normalize(cross(tanU * mirrorSize, tanV * mirrorSize)), h);
}

float mirrorBRDF(vec3 inDir, vec3 outDir, vec3 normal) {
    vec3 halfway = (inDir + outDir) / 2.0;
    return pow(max(dot(halfway, normal), 0), mirrorShininess);
}

void main()
{
    vec3 wCanvasPos = vec3(2 * texCoords - 1, 0);
    vec3 wCanvasNormal = vec3(0, 0, -1);
    float I = 0.0;
    for (int x = 0; x < resolution; x++) {
        for (int y = 0; y < resolution; y++) {
            vec2 mirrorUV = texCoords + traceAreaScale * vec2(x, y) / float(resolution) - traceAreaScale * 0.5;
            vec4 normHeight = normHeight(mirrorUV);
            vec3 mMirrorPos = vec3(2 * mirrorUV - 1, normHeight.w);
            vec3 wMirrorPos = mirrorSize * mMirrorPos - vec3(0, 0, mirrorDistance);
            vec3 wMirrorNormal = normHeight.xyz;
            vec3 toMirror = normalize(wMirrorPos - wCanvasPos);
            vec3 ld = normalize(lightDir);
            I += dot(toMirror, wCanvasNormal) * mirrorBRDF(ld, -toMirror, wMirrorNormal) * lightIntensity * dot(ld, wMirrorNormal) * mirrorSize.x * mirrorSize.y * pow(traceAreaScale * 2.0 / float(resolution), 2) / pow(length(wMirrorPos - wCanvasPos), 2);
        }
    }
    I = min(I, 1.0);
    vec3 color = vec3(pow(I, 3) * vec3(1,1,0) + (pow(I, 2) - pow(I, 3)) * vec3(1,0,0) + (pow(I, 1) - pow(I, 2) - pow(I, 3)) * vec3(0,0,1));
    const float gamma = 0.7;
    const float exposure = 2.0;

    // exposure correction 
    FragColor = vec4(vec3(1.0) - exp(-color * exposure), 1.0);
    //FragColor = vec4(normHeight(texCoords).xyz, 1.0);
}