#version 420 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
	vec4 worldPos;
	vec2 texCoords;
	mat3 TBN;		// Tangent-Bitangent-Normal matrix
} gs_in[];

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
};


void generateLine (int idx) {
	int id1 = (idx < 2)? idx + 1 : 0;
	int id2 = (idx < 1)? idx + 2 : ((idx < 2)? 0 : 1);
	gl_Position = gl_in[idx].gl_Position;
	EmitVertex();
	gl_Position = viewProjMatrix * (gs_in[idx].worldPos 
									+ vec4(0.2 * normalize(cross(gs_in[id2].worldPos.xyz / gs_in[id2].worldPos.w - gs_in[idx].worldPos.xyz / gs_in[idx].worldPos.w,
									gs_in[id1].worldPos.xyz / gs_in[id1].worldPos.w - gs_in[idx].worldPos.xyz / gs_in[idx].worldPos.w)), 0.0)
								);
	EmitVertex();
	EndPrimitive();
}

void main () {
	generateLine(0);
	generateLine(1);
	generateLine(2);
}