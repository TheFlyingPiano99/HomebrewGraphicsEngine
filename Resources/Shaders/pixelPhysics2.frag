#version 420 core

layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform sampler2D inputTexture;

uniform float deltaTime;
uniform bool initPhase;

in VS_OUT {
	vec4 worldPos;
	vec2 texCoords;
	mat3 TBN;		// Tangent-Bitangent-Normal matrix
} fs_in;

bool isEqual(vec3 a, vec3 b) {
	return abs(a.r - b.r) < 0.01 && abs(a.g - b.g) < 0.01 && abs(a.b - b.b) < 0.01;
}

bool checkIsGrounded(vec2 pos) {
	vec3 ground = vec3(0,1,0);
	vec3 empty = vec3(0,0,0);
	vec2 resolution = textureSize(inputTexture, 0);
	bool isGrounded = true;
	for (float y = pos.y - 1.0 / resolution.y; y > 0.0; y = y - 1.0 / resolution.y ) {
		vec3 c = texture(inputTexture, vec2(pos.x, y)).rgb;
		if (isEqual(c, empty)) {
			isGrounded = false;
			break;
		}
		else if (isEqual(c, ground)) {
			break;
		}
	}
	return isGrounded;
}

void main()
{
	vec3 water = vec3(0,0,1);
	vec3 ground = vec3(0,1,0);
	vec3 empty = vec3(0,0,0);
	vec3 blood = vec3(1,0,0);
	vec3 current = texture(inputTexture, fs_in.texCoords).rgb;
	if (initPhase) {
		if (fs_in.texCoords.x < 0.52 && fs_in.texCoords.y < 0.41
			&& fs_in.texCoords.x > 0.4 && fs_in.texCoords.y > 0.35) {
			current = water;
			FragColor = vec4(current.rgb, 1.0);
			return;
		}
		if (fs_in.texCoords.x < 0.54 && fs_in.texCoords.y < 0.2
			&& fs_in.texCoords.x > 0.3 && fs_in.texCoords.y > 0.19) {
			current = ground;
			FragColor = vec4(current.rgb, 1.0);
			return;
		}
		if (fs_in.texCoords.y < 0.01
			&& fs_in.texCoords.y > 0.0) {
			current = ground;
			FragColor = vec4(current.rgb, 1.0);
			return;
		}
		if (fs_in.texCoords.y < 0.1
			&& fs_in.texCoords.y > 0.0
			&& (fs_in.texCoords.x < 0.01 || fs_in.texCoords.x > 0.99)) {
			current = ground;
			FragColor = vec4(current.rgb, 1.0);
			return;
		}
		FragColor = vec4(0,0,0, 1.0);
		return;
	}
	vec2 resolution = textureSize(inputTexture, 0);
	
	vec2 currentPos = fs_in.texCoords;
	vec2 leftPos = fs_in.texCoords + vec2(-1.0, 0.0) / resolution;
	vec2 leftLeftPos = fs_in.texCoords + vec2(-2.0, 0.0) / resolution;
	vec2 rightPos = fs_in.texCoords + vec2(1.0, 0.0) / resolution;
	vec2 rightRightPos = fs_in.texCoords + vec2(2.0, 0.0) / resolution;
	vec2 rightRightUpPos = fs_in.texCoords + vec2(2.0, 1.0) / resolution;
	vec2 downPos = fs_in.texCoords + vec2(0.0, -1.0) / resolution;
	vec2 upPos = fs_in.texCoords + vec2(0.0, 1.0) / resolution;

	vec3 up = texture(inputTexture, fs_in.texCoords + vec2(0.0, 1.0) / resolution).rgb;
	vec3 upUp = texture(inputTexture, fs_in.texCoords + vec2(0.0, 2.0) / resolution).rgb;
	vec3 down = texture(inputTexture, fs_in.texCoords + vec2(0.0, -1.0) / resolution).rgb;
	vec3 downDown = texture(inputTexture, fs_in.texCoords + vec2(0.0, -2.0) / resolution).rgb;
	vec3 rightUp = texture(inputTexture, fs_in.texCoords + vec2(1.0, 1.0) / resolution).rgb;
	vec3 leftUp = texture(inputTexture, fs_in.texCoords + vec2(-1.0, 1.0) / resolution).rgb;
	vec3 rightDown = texture(inputTexture, fs_in.texCoords + vec2(1.0, -1.0) / resolution).rgb;
	vec3 leftDown = texture(inputTexture, fs_in.texCoords + vec2(-1.0, -1.0) / resolution).rgb;
	vec3 leftLeftDown = texture(inputTexture, fs_in.texCoords + vec2(-2.0, -1.0) / resolution).rgb;
	vec3 leftLeftUp = texture(inputTexture, fs_in.texCoords + vec2(-2.0, 1.0) / resolution).rgb;
	vec3 right = texture(inputTexture, fs_in.texCoords + vec2(1.0, 0.0) / resolution).rgb;
	vec3 left = texture(inputTexture, fs_in.texCoords + vec2(-1.0, 0.0) / resolution).rgb;
	vec3 rightUpUp = texture(inputTexture, fs_in.texCoords + vec2(1.0, 2.0) / resolution).rgb;
	vec3 leftUpUp = texture(inputTexture, fs_in.texCoords + vec2(-1.0, 2.0) / resolution).rgb;
	vec3 rightRight = texture(inputTexture, fs_in.texCoords + vec2(2.0, 0.0) / resolution).rgb;
	vec3 rightRightUp = texture(inputTexture, fs_in.texCoords + vec2(2.0, 1.0) / resolution).rgb;
	vec3 leftLeft = texture(inputTexture, fs_in.texCoords + vec2(-2.0, 0.0) / resolution).rgb;

	vec3 color = current;
	
	// Current particle:
	if (isEqual(current, water)) {
		if (
			!isEqual(up, water) && isEqual(down, empty) && !(isEqual(leftUp, water) && !isEqual(left, empty))
			&& !(isEqual(rightUp, water) && !isEqual(right, empty) && )
		) {
			color = empty;
		}
	}
	
	// Around particle:
	else if (isEqual(current, empty)) {
	}
	
	color = min(color, vec3(1,1,1));
	FragColor = vec4(color, 1.0);
}