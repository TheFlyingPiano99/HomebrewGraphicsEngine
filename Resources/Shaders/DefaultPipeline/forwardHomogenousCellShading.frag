#version 460 core


in VS_OUT {
	vec4 worldPos;
	vec2 texCoords;
	mat3 TBN;		// Tangent-Bitangent-Normal matrix
} fs_in;

layout (location = 0) out vec4 FragColor;

uniform struct Material {
	vec3 albedo;
	float roughness;
	float metallic;
	float ao;
} material;

layout (std140, binding = 0) uniform Camera {	// base alignment	aligned offset
	vec3 cameraPosition;			// 16				0
	mat4 viewProjMatrix;			// 16				16				(column 0)
									// 16				32				(column 1)
									// 16				48				(column 2)
									// 16				64				(column 3)
	mat4 rayDirMatrix;				// 16				80				(column 0)
									// 16				96				(column 1)
									// 16				192				(column 2)
									// 16				208				(column 3)
} camera;

layout (std140) struct Light {
	vec4 position;		
	vec3 powerDensity;
	vec4 shadowMapIdx;	// -1 if no shadow map
};

layout (std140, binding = 1) uniform Lights {			// aligned size: 32 bytes
	int lightCount;
	Light lights[2048];
};


void main()
{
	//vec3 kd = texture(sprite, fs_in.texCoords).rgb;
	vec3 kd = material.albedo / 3.14159265359;
	vec3 ks = vec3(0.1, 0.1, 0.1);
	vec3 ambient = vec3(3.0);
	float shininess = 100.0;

	vec3 sumL = vec3(0);
	
	sumL += kd * ambient;
	vec3 n = normalize(fs_in.TBN * vec3(0, 0, 1));
	vec3 v = normalize(camera.cameraPosition - fs_in.worldPos.xyz);
	float ndotv = dot(n, v);

	if (abs(ndotv) < 0.01) {	// silhouette
		FragColor = vec4(0,0,0,1);
		return;
	}
	
	for (int idx = 0; idx < lightCount; idx++) {
		vec3 toLight =  lights[idx].position.xyz - fs_in.worldPos.xyz * lights[idx].position.w;
		vec3 l = normalize(toLight);
		float distanceSquared = dot(toLight, toLight);
		float ndotl = dot(n, l);
		vec3 h = normalize(l + v);
		sumL += lights[idx].powerDensity / distanceSquared * max(ndotl, 0.0) *
			(
			kd
			
			+ ks * pow(max(dot(n, h), 0.0), shininess) / (max(max(ndotl, ndotv), 0.0) + 0.00001)
			);
	}

	FragColor = vec4(vec3(ivec3(vec3(min(sumL.r, 1.0), min(sumL.g, 1.0), min(sumL.b, 1.0)) * 6.0)) / 6.0, 1);
}