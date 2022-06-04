#version 420 core

out vec4 FragColor;
in vec2 texCoords;

layout(binding=0) uniform sampler2D screenColorTexture;
layout(binding=1) uniform sampler2D screenBrightColorTexture;
layout(binding=2) uniform sampler2D screenDepthStencilTexture;
layout(binding=3) uniform sampler2D shadowDepthTexture;


uniform int windowWidth;
uniform int windowHeight;

float widthOffset = 1.0 / 2000.0f;
float heightOffset = 1.0 / 2000.0f;

#define USED_KERNEL_SIZE 9
#define NUMBER_OF_STARS 750

uniform vec3 stars[NUMBER_OF_STARS];
uniform vec3 starColor;

// Itt volt compile error 
// Eddig az init így nézett ki: vec2 myVec2[n] = { ... };
// A fix: vec2 myVec2[n] = vec2[]( ... );
vec2 adjacentOffset[9] = vec2[](
	vec2(-widthOffset, -heightOffset),	vec2(0, -heightOffset), vec2(widthOffset, -heightOffset),
	vec2(-widthOffset, 0),				vec2(0, 0),				vec2(widthOffset, 0),
	vec2(-widthOffset, heightOffset),	vec2(0, heightOffset),	vec2(widthOffset, heightOffset)
);

vec2 surroundingOffset[25] = vec2[](
	2 * vec2(-widthOffset, -heightOffset),	2 * vec2(-widthOffset, -heightOffset),	2 * vec2(0, -heightOffset),		vec2(widthOffset, -heightOffset),	2 * vec2(widthOffset, -heightOffset),
	2 * vec2(-widthOffset, -heightOffset),		vec2(-widthOffset, -heightOffset),		vec2(0, -heightOffset),		vec2(widthOffset, -heightOffset),	2 * vec2(widthOffset, -heightOffset),
	2 * vec2(-widthOffset, 0),					vec2(-widthOffset, 0),					vec2(0, 0),					vec2(widthOffset, 0),				2 * vec2(widthOffset, 0),
	2 * vec2(-widthOffset, heightOffset),		vec2(-widthOffset, heightOffset),		vec2(0, heightOffset),		vec2(widthOffset, heightOffset),	2 * vec2(widthOffset, heightOffset),
	2 * vec2(-widthOffset, heightOffset),	2 * vec2(-widthOffset, heightOffset),	2 * vec2(0, heightOffset),	2 * vec2(widthOffset, heightOffset),	2 * vec2(widthOffset, heightOffset)
);


float blurKernel[9] = float[](
	1/9.0,		1/9.0,		1/9.0,
	1/9.0,		1/9.0,		1/9.0,
	1/9.0,		1/9.0,		1/9.0
);

float greaterBlurKernel[25] = float[](
	1/25.0,		1/25.0,		1/25.0,		1/25.0,		1/25.0,
	1/25.0,		1/25.0,		1/25.0,		1/25.0,		1/25.0,
	1/25.0,		1/25.0,		1/25.0,		1/25.0,		1/25.0,
	1/25.0,		1/25.0,		1/25.0,		1/25.0,		1/25.0,
	1/25.0,		1/25.0,		1/25.0,		1/25.0,		1/25.0
);

float outlineKernel[9] = float[](
	1, 1, 1,
	1, -8, 1,
	1, 1, 1
);

struct Camera {
	vec3 eye;
	vec3 center;
	vec3 up;
	vec3 right;
	float FOVrad;
	float aspectRatio;
	mat4 Mat;
	mat4 invMat;
};
uniform Camera camera;
uniform Camera lightCamera;

struct Atmosphere {
	vec3 center;
	float radius;
	float planetRadius;
	

	vec3 rayleighScattering;
	float mieScattering;
	float heightOfAverageDensity;


	vec3 quadraticAbsorption;
	vec3 linearAbsorption;
	vec3 constantAbsorption;
	vec3 quadraticScattering;
	vec3 linearScattering;
	vec3 constantScattering;
	vec3 quadratiReflectiveness;
	vec3 linearReflectiveness;
	vec3 constantReflectiveness;
	float quadratiDensity;
	float linearDensity;
	float constantDensity;
};
uniform Atmosphere atmosphere;

struct Sun {
	vec3 position;
	vec3 color;
};
uniform Sun sun;

uniform float exposure;
uniform float gamma;

float near = 0.1f;
float far = 200.0f;

float linearizeDepth(float depth) {
	return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

float logisticDepth(float depth, float steepness, float offset) {
	float zVal = linearizeDepth(depth);
	return 1 / (1 + exp(-steepness * (zVal - offset)));
}

vec3 decodeLocation(mat4 invMat, float depthValue, vec2 texCoord)
{
  vec4 clipSpaceLocation;
  clipSpaceLocation.xy = texCoord * 2.0f - 1.0f;
  clipSpaceLocation.z = depthValue * 2.0f - 1.0f;
  clipSpaceLocation.w = 1.0f;
  vec4 homogenousLocation = invMat * clipSpaceLocation;
  return homogenousLocation.xyz / homogenousLocation.w;
}

vec2 calculateLightCameraTexCoord(vec3 point) {
	vec4 transformed = lightCamera.Mat * vec4(point, 1.0);
	return vec2(transformed.xy / transformed.w) / 2.0f + 0.5f;
}

vec3 postprocess(vec2 offset[USED_KERNEL_SIZE], float kernel[USED_KERNEL_SIZE], sampler2D sampleTexture, vec2 texCoord) {
	vec3 color = vec3(0.0);
	for (int i = 0; i < USED_KERNEL_SIZE; i++) {
		vec2 offsettedTexCoord = vec2(max(min((texCoord + offset[i]).x, 1), 0), max(min((texCoord + offset[i]).y, 1), 0));
		color += (texture(sampleTexture, offsettedTexCoord).xyz) * kernel[i];
	}
	return color;
}

uniform float gaussWeights[10] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216, 0.005, 0.004, 0.003, 0.002, 0.001);

vec3 calculateBloom() {
	int amount = 10;
	vec3 bloom = vec3(0.0);
	vec2 offset = 1.0 / textureSize(screenBrightColorTexture, 0);
	for (int x = 0; x < amount; x++) {
		for (int y = 0; y < amount; y++) {
			bloom += texture(screenBrightColorTexture,
				texCoords
				+ vec2(offset.x, 0.0) * x
				+ vec2(0.0, offset.y) * y).rgb
				* gaussWeights[x]
				* gaussWeights[y];
		}
		for (int y = -1; y > -amount; y--) {
			bloom += texture(screenBrightColorTexture,
				texCoords
				+ vec2(offset.x, 0.0) * x
				+ vec2(0.0, offset.y) * y).rgb
				* gaussWeights[x]
				* gaussWeights[-y];
		}
	}
	for (int x = -1; x > -amount; x--) {
		for (int y = 0; y < amount; y++) {
			bloom += texture(screenBrightColorTexture,
				texCoords
				+ vec2(offset.x, 0.0) * x
				+ vec2(0.0, offset.y) * y).rgb
				* gaussWeights[-x]
				* gaussWeights[y];
		}
		for (int y = -1; y > -amount; y--) {
			bloom += texture(screenBrightColorTexture,
				texCoords
				+ vec2(offset.x, 0.0) * x
				+ vec2(0.0, offset.y) * y).rgb
				* gaussWeights[-x]
				* gaussWeights[-y];
		}
	}
	return bloom;
}

void calculateRayStart(vec2 normalCameraCoord, out vec3 rayStart, out vec3 rayDir) {
	float scale = tan(camera.FOVrad / 2.0);
	rayStart = camera.center
			+ camera.right * camera.aspectRatio * scale * normalCameraCoord.x
			+ camera.up * scale * normalCameraCoord.y;
	rayDir = normalize(rayStart - camera.eye);
	rayStart = camera.eye;
}

bool solveQuadratic(float a, float b, float c, out float x1, out float x2) {
	float discriminant = b * b - 4 * a * c;
	if (discriminant < 0.0) {
		return false;
	}
	x1 = (-b + sqrt(discriminant)) / (2.0 * a);
	x2 = (-b - sqrt(discriminant)) / (2.0 * a);
	return true;
}

bool intersectSphere(vec3 rayDir, vec3 rayPos, vec3 sphereCenter, float sphereRadius, out float shortDist, out float longDist){
	float a = dot(rayDir, rayDir);
	float b = 2.0 * (dot(rayDir, rayPos - sphereCenter)); 
	float c = dot(rayPos - sphereCenter, rayPos - sphereCenter) - sphereRadius * sphereRadius;
	return solveQuadratic(a, b, c, longDist, shortDist);
}

bool intersectCone(vec3 rayDir, vec3 rayPos, vec3 coneTip, vec3 coneDir, float coneHalfAngle, out float shortDist, out float longDist){
	float DdotV = dot(rayDir, coneDir);
	float cosTheta =cos(coneHalfAngle);
	float cosThetaSquare = cosTheta * cosTheta;
	vec3 CO = rayPos - coneTip;
	float a = DdotV * DdotV - cosThetaSquare;
	float b = 2.0 * (DdotV * dot(CO, coneDir) - dot(rayDir, CO * cosThetaSquare)); 
	float c = dot(CO, coneDir) * dot(CO, coneDir) - dot(CO, CO * cosThetaSquare);
	return solveQuadratic(a, b, c, longDist, shortDist);
}

vec3 calculateAtmosphere() {
	vec3 color = vec3(0.0);
	vec3 rayStart;
	vec3 rayDirection;
	calculateRayStart(texCoords * 2 - 1, rayStart, rayDirection);
	
	//IntersectAtmosphere	

	float shortDist, longDist;
	if (!intersectSphere(rayDirection, rayStart, atmosphere.center, atmosphere.radius, shortDist, longDist)) {
		return vec3(0.0);	// No intersection with the atmosphere!
	}
	shortDist = max(shortDist, 0);
	longDist = max(longDist, 0);

	// Test intersection with the planet located in atmosphere:
	/*
	float shortPlanetDist, longPlanetDist;
	if (intersectSphere(rayDirection, rayStart, atmosphere.center, atmosphere.planetRadius, shortPlanetDist, longPlanetDist)) {
		if (longDist > longPlanetDist && longPlanetDist > 0) {
			longDist = longPlanetDist;
		}
		if (longDist > shortPlanetDist && shortPlanetDist > 0) {
			longDist = shortPlanetDist;
		}
	}
	*/

	float bufferDepth = linearizeDepth(texture(screenDepthStencilTexture, texCoords).x);
	if (bufferDepth < longDist) {
		longDist = bufferDepth;
	}
	if (shortDist > longDist) {
		return vec3(0.0);
	}

	float travel = longDist - shortDist;
	//float normalTravel = travel / (2.0 * atmosphere.radius);
	int maxStep = 50;
	float stepSize = travel / (1.0 * maxStep + 1);
	vec3 rayPosition = rayStart + rayDirection * (shortDist + stepSize);
	float depthInatmosphereToEye = stepSize;
	for (int i = 0; i < maxStep; i++) {
		vec3 sunDir = normalize(sun.position - rayPosition);
		float shortDistToSun, longDistToSun;
		if (!intersectSphere(sunDir, rayPosition, atmosphere.center, atmosphere.radius, shortDistToSun, longDistToSun)) {
			continue;
		}
		shortDistToSun = max(shortDistToSun, 0);
		longDistToSun = max(longDistToSun, 0);
		float shortPlanetDistToSun, longPlanetDistToSun;
		bool inShadow = false;
		if (intersectSphere(sunDir, rayPosition, atmosphere.center, atmosphere.planetRadius, shortPlanetDistToSun, longPlanetDistToSun)) {
			if (shortPlanetDistToSun > 0 || longPlanetDistToSun > 0) {
				inShadow = true;	// Sun behind planet
			}
			else if (shortPlanetDistToSun < 0 && longPlanetDistToSun > 0) {
				inShadow = true;	// Inside planet
			}
		}
		float depthInatmosphereToSun =  longDistToSun - shortDistToSun;
		if (!inShadow) {
			float fullDepth = depthInatmosphereToSun + depthInatmosphereToEye;
			//float distanceFromAtmEdgeToSun = length(rayPosition + dToSunFront * sunDir - sun.position);
			float d = max(atmosphere.radius - length(rayPosition - atmosphere.center), 0.0);
			float density = d * d * atmosphere.quadratiDensity + d * atmosphere.linearDensity + atmosphere.constantDensity; 
			float scaterAngle = max(dot(sunDir, rayDirection), 0);
			float reflectAngle = max(dot(sunDir, -rayDirection), 0);
			color += sun.color /* / (distanceFromAtmEdgeToSun * distanceFromAtmEdgeToSun)*/ * density * stepSize
					* (scaterAngle * scaterAngle * atmosphere.quadraticScattering + scaterAngle * atmosphere.linearScattering + atmosphere.constantScattering)
					* (reflectAngle * reflectAngle * atmosphere.quadratiReflectiveness + reflectAngle * atmosphere.linearReflectiveness + atmosphere.constantReflectiveness)
					/ (fullDepth * fullDepth * atmosphere.quadraticAbsorption
					+ fullDepth * atmosphere.linearAbsorption
					+atmosphere.constantAbsorption);
		}

		rayPosition += rayDirection * stepSize;
		depthInatmosphereToEye += stepSize;
	}

	// Checks if any of the color values are outside of the displayable values and displays white instead of em.
	// We can check if we need HDR (if any white spaces show up)
	/*if (color.r > 1.0 || color.g > 1.0 || color.b > 1.0)
		return vec3(1, 1, 1);*/

	return max(color, 0);
}

vec3 calculateStars(float atmosphereIntensity) {
	if (texture(screenDepthStencilTexture, texCoords).x < 1.0) {
		return vec3(0);
	}
	vec3 rayStart;
	vec3 rayDirection;
	calculateRayStart(texCoords * 2 - 1, rayStart, rayDirection);

	for (int i = 0; i < NUMBER_OF_STARS / 2; i++) {
		float cosTheta = dot(stars[i], rayDirection);
		if (cosTheta > 0.999999f) {
			if (atmosphereIntensity > 0.0f) {
				return starColor / (atmosphereIntensity * 50);
			}
			else {
				return starColor;
			}
		}
	}
	return vec3(0);
}

// ---------------- EXPONENTIAL OPTICAL DEPTH START ----------------
float densityFalloff = 3.0f;

// does it intersect planet? if it dont set planetvalue to 10000000000000000000
// does it intesect atmosphere? if it dont intersect either return -1
float sunRayLengthThroughAtmosphere(vec3 rayStart, vec3 rayDir)
{
	float nearAtmDist, farAtmDist;
	vec2 lightTexCoords = calculateLightCameraTexCoord(rayStart);
	vec3 bufferData = postprocess(adjacentOffset, blurKernel, shadowDepthTexture, lightTexCoords);

	if (intersectSphere(rayDir, rayStart, atmosphere.center, atmosphere.radius, nearAtmDist, farAtmDist)) {
		vec3 depthBufferPoint = decodeLocation(lightCamera.invMat, bufferData.r, lightTexCoords);
		float depth; 
		float distToSun = length(sun.position - rayStart);
		if (dot(depthBufferPoint - sun.position, -rayDir) > 0.0f) {	// In front of camera
			depth = length(depthBufferPoint - sun.position);
		}
		else {		// Behind camera
			depth = -1;
		}

		if (depth > 0.0f && depth < distToSun) {	// Object between Sun and rayStart
			return -1.0f;
		}
		if (nearAtmDist < 0.0f) {
			nearAtmDist = 0.0f;
		}
		if (farAtmDist < nearAtmDist) {
			return -1.0f;
		}
		return farAtmDist - nearAtmDist;
	}
	return -1.0f;
}

/*
	Zoli trying his best.
*/
float rayLengthThroughAtmosphereWithDepthBuffer(vec3 rayStart, vec3 rayDir, out vec3 startPosInAtmosphere) {
	float nearAtmDist, farAtmDist;
	if (intersectSphere(rayDir, rayStart, atmosphere.center, atmosphere.radius, nearAtmDist, farAtmDist)) {
		vec3 depthBufferPoint = decodeLocation(camera.invMat, texture(screenDepthStencilTexture, texCoords).r, texCoords);
		float depth; 
		if (dot(depthBufferPoint - rayStart, rayDir) > 0.0f) {	// In front of camera
			depth = length(depthBufferPoint - rayStart);
		}
		else {		// Behind camera
			depth = -1;
		}

		if (depth > 0.0f && depth < nearAtmDist) {	// View is obscured.
			return -1.0f;
		}
		else if (depth > 0.0f && nearAtmDist > 0.0f && depth > nearAtmDist && depth < farAtmDist) {	// Watching from space and seeing the planet.
			startPosInAtmosphere = rayStart + rayDir * nearAtmDist;
			return depth - nearAtmDist;
		}
		else if (depth > 0.0f && depth < farAtmDist) {	// Camera inside atmosphere and seeing the planet.
			startPosInAtmosphere = rayStart;
			return depth;
		}
		else if (nearAtmDist > 0.0f) {	// Watching from space.
			startPosInAtmosphere = rayStart + rayDir * nearAtmDist;
			return farAtmDist - nearAtmDist;
		}
		else if (farAtmDist > 0.0f) {	// Camera inside atmosphere.
			startPosInAtmosphere = rayStart;
			return farAtmDist;
		}

	}

	return -1.0f;
}


float densityAtPoint(vec3 point)
{
	// height above planet surface (vec3(0.0, 0.0, 0.0) = center, 3.0 = radius)
	float heightAboveSurface = length(point - atmosphere.center) - atmosphere.planetRadius;
	// correcting the exp function so it intersects at y = 0, x = 1
	float height01 = heightAboveSurface / (atmosphere.radius - atmosphere.planetRadius);
	// return local density
	return exp(-height01 * densityFalloff) * (1 - height01);
}

int opticalDepthPointNumber = 10;

vec3 outScattering(vec3 rayOrigin, vec3 rayDir, float rayLength, vec3 scattering)
{
	vec3 densitySamplePoint = rayOrigin;
	float stepSize = rayLength / (opticalDepthPointNumber - 1);
	vec3 opticalDepth = vec3(0.0f);
	for (int i = 0; i < opticalDepthPointNumber; i++)
	{
		float localDensity = densityAtPoint(densitySamplePoint);

		opticalDepth += localDensity * stepSize;
		densitySamplePoint += rayDir * stepSize;
	}
	
	return 4.0f * 3.14f * scattering * opticalDepth;
}

float phaseFunction(float cosTheta, float g) {
	return (3 * (1 - g * g)) / (2 * (2 + g * g))
	* (1 + cosTheta * cosTheta) / pow(1 + g * g - 2 * g * cosTheta, 1.5);
}

int inScatterPointNumber = 10;

vec3 inScatteringCore(vec3 inScatterPoint, vec3 viewDir, vec3 sunDir, float viewRayLength, float sunRayLength, vec3 scattering, float g) {
	// get the optical depth to and from the inscatterPoint
	vec3 sunRayOpticalDepth = outScattering(inScatterPoint, sunDir, sunRayLength, scattering);
	vec3 viewRayOpticalDepth = outScattering(inScatterPoint, -viewDir, viewRayLength, scattering);
	// calculate transmittance according to the optical depth
	vec3 transmittance = exp(-(sunRayOpticalDepth + viewRayOpticalDepth));
	// get the local density at the inScatterPoint
	float localDensity = densityAtPoint(inScatterPoint);
	float phase = phaseFunction(dot(sunDir, -viewDir), g);
	return scattering * phase * localDensity * transmittance;
}

vec3 inScattering(vec3 viewRayStartPos, vec3 viewDir, float viewRayLength, float initialViewRayLengthTraveled) {
	vec3 inScatterPoint = viewRayStartPos;
	vec3 inScatteredLight = vec3(0.0f);
	float distanceTraveled = initialViewRayLengthTraveled;
	float stepSize = viewRayLength / (inScatterPointNumber);
	while(distanceTraveled < viewRayLength)
	{
		// calculateDirToSun
		vec3 sunDir = normalize(sun.position - inScatterPoint);
		vec3 startPos;	// Not used.
		// calculate the distance to the sun from the in scatter point
		float sunRayLength = sunRayLengthThroughAtmosphere(inScatterPoint, sunDir);
		// should not do anything if the inScatterPoint is in shade
		if (sunRayLength > 0.0f) {
			vec3 rayleigh = inScatteringCore(inScatterPoint, viewDir, sunDir, distanceTraveled, sunRayLength, atmosphere.rayleighScattering, 0.0f);
			vec3 mie = inScatteringCore(inScatterPoint, viewDir, sunDir, distanceTraveled, sunRayLength, vec3(atmosphere.mieScattering), -0.99f);
			inScatteredLight += (rayleigh + mie) * stepSize;
		}
		inScatterPoint += viewDir * stepSize;
		distanceTraveled += stepSize;
	}
	return sun.color * inScatteredLight;
}



vec3 calculateLight(vec3 startPosInAtmosphere, vec3 viewDir, float viewRayLength)
{	
//	bool prevWasInShadow = false;
/*
	float nearConeDist, farConeDist;
	bool coneIntersected = false;	// No need to calculate intersection twice.
	bool insideCone = false;
	bool inShadow = false;
	float coneHalfAngle = atan(atmosphere.planetRadius / length(atmosphere.center - sun.position));
	vec3 coneAxis = normalize(atmosphere.center - sun.position);
	coneIntersected = intersectCone(viewDir, startPosInAtmosphere, sun.position, coneAxis, coneHalfAngle, nearConeDist, farConeDist);
	if (coneIntersected) {	// More precise testing
		if (near > far) {
			float temp = near;
			near = far;
			far = temp;
		}
		float axisAngle = dot(coneAxis, startPosInAtmosphere + nearConeDist * viewDir - sun.position);
		float nearAngle = dot(coneAxis, startPosInAtmosphere + nearConeDist * viewDir - atmosphere.center);
		float farAngle = dot(coneAxis, startPosInAtmosphere + farConeDist * viewDir - atmosphere.center);
		float posAngle = dot(startPosInAtmosphere - atmosphere.center, coneAxis);
		float viewAngle = dot(coneAxis, viewDir);
		if (axisAngle < 0.0f) {
			coneIntersected = false;
		}
		else if (farConeDist < 0.0f) {	// The shadow cone is behind the camera.
			coneIntersected = false;			
		}
		else if (nearAngle < 0.0f || farAngle < 0.0f) {	// Not behind planet.
			coneIntersected = false;
		}
		else if (nearConeDist > viewRayLength) {	// The shadow cone is behind the visible range.
			coneIntersected = false;			
		}
		else if (nearConeDist < 0.0f && farConeDist > 0.0f && posAngle < 0.0f) {// Camera inside cone
			coneIntersected = false;
		}
		else if (nearConeDist < 0.0f && farConeDist > 0.0f && farAngle > 0.0f) {
			insideCone = true;
			if (farConeDist > viewRayLength) {
				farConeDist = viewRayLength;
			}
		}
		else {
		}
	}

*/
	return inScattering(startPosInAtmosphere, viewDir, viewRayLength, 0);
/*
	if (insideCone) {
		return inScattering(startPosInAtmosphere + farConeDist * viewDir, viewDir, viewRayLength - farConeDist, farConeDist);
	}
	else if (coneIntersected) {
		return inScattering(startPosInAtmosphere, viewDir, nearConeDist, 0)	//Till shadow
			+ inScattering(startPosInAtmosphere + nearConeDist * viewDir, viewDir, viewRayLength - farConeDist, farConeDist);	//After shadow
	}
	else if (!inShadow) {
		return inScattering(startPosInAtmosphere, viewDir, viewRayLength, 0);
	}
*/
}
// ---------------- EXPONENTIAL OPTICAL DEPTH END ----------------

void main() {
	vec3 hdrColor;
	/*
	vec3 atmosphere = calculateAtmosphere();
	// NO HDR
	//FragColor = vec4(texture(screenColorTexture, texCoords).xyz + atmosphere + calculateStars(length(atmosphere)), 1.0);
	
	//With HDR
	hdrColor = vec4(texture(screenColorTexture, texCoords).xyz + atmosphere + calculateStars(length(atmosphere)), 1.0).rgb;
	*/
	
	//--------------------------------------------------------------------------------------

	// FALIED EXPONENTIAL EXPERIMENTATION
	//FragColor = vec4(texture(screenColorTexture, texCoords).xyz, 1.0f);


	vec3 cameraRayStart;
	vec3 cameraRayDirection;
	calculateRayStart(texCoords * 2 - 1, cameraRayStart, cameraRayDirection);
	vec3 bloom = calculateBloom() * 0.01;
	vec3 startPosInAtmosphere;
	float rayLengthOfViewRay = rayLengthThroughAtmosphereWithDepthBuffer(cameraRayStart, cameraRayDirection, startPosInAtmosphere);
	if (rayLengthOfViewRay > 0.0f)	// Calculate the atmosphere
	{
		vec3 atmosphereColor = calculateLight(startPosInAtmosphere, cameraRayDirection, rayLengthOfViewRay);
		vec3 stars = calculateStars(length(atmosphereColor));
		hdrColor = vec4(texture(screenColorTexture, texCoords).rgb + bloom + atmosphereColor + stars, 1.0f).rgb;
	}
	else	// Calculate no atmosphere
	{
		vec3 stars = calculateStars(0);
		hdrColor = vec4(texture(screenColorTexture, texCoords).rgb + bloom  + stars, 1.0f).rgb;
	}

	// HDR Tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

	// GAMMA CORRECTION (OPTIONAL)
    result = pow(result, vec3(1.0 / gamma));

	FragColor = vec4(result, 1.0f);

}