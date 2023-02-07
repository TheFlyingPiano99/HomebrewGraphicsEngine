#include "GeometryFactory.h"
#include <complex>
#include "GlobalInclude.h"

namespace Hogra {

	GeometryFactory* GeometryFactory::instance = nullptr;

	Geometry* GeometryFactory::GetSphere()
	{
		if (!Allocator::IsValid(sphere)) {
			sphere = GenerateSphere();
			sphere->SetFaceCulling(true);
		}
		return sphere;
	}

	Geometry* GeometryFactory::GetWireFrameSphere()
	{
		if (!Allocator::IsValid(wireFrameSphere)) {
			std::vector<Vertex_pos_norm_tang_bitang_uv> vertices;
			std::vector<GLuint> indices;
			int zRes = 100;
			int yRes = 100;
			int xRes = 100;
			float r = 1.0f;
			for (int i = 0; i < zRes; i++) {
				Vertex_pos_norm_tang_bitang_uv v;
				v.position = r * glm::vec3(std::sinf(2.0f * M_PI * i / (float)zRes), std::cosf(2.0f * M_PI * i / (float)zRes), 0.0f);
				vertices.push_back(v);

				if (i < zRes - 1) {
					indices.push_back(i);
					indices.push_back(i + 1);
				}
				else {
					indices.push_back(i);
					indices.push_back(0);
				}
			}
			for (int i = 0; i < yRes; i++) {
				Vertex_pos_norm_tang_bitang_uv v;
				v.position = r * glm::vec3(std::sinf(2.0f * M_PI * i / (float)yRes), 0.0f, std::cosf(2.0f * M_PI * i / (float)yRes));
				vertices.push_back(v);
				if (i < yRes - 1) {
					indices.push_back(zRes + i);
					indices.push_back(zRes + i + 1);
				}
				else {
					indices.push_back(zRes + i);
					indices.push_back(zRes + 0);
				}
			}
			for (int i = 0; i < xRes; i++) {
				Vertex_pos_norm_tang_bitang_uv v;
				v.position = r * glm::vec3(0.0f, std::sinf(2.0f * M_PI * i / (float)xRes), std::cosf(2.0f * M_PI * i / (float)xRes));
				vertices.push_back(v);
				if (i < xRes - 1) {
					indices.push_back(zRes + yRes + i);
					indices.push_back(zRes + yRes + i + 1);
				}
				else {
					indices.push_back(zRes + yRes + i);
					indices.push_back(zRes + yRes + 0);
				}
			}
			wireFrameSphere = Allocator::New<Geometry>();
			wireFrameSphere->Init(vertices, indices);
			wireFrameSphere->SetPrimitiveType(GL_LINES);
			wireFrameSphere->SetFaceCulling(false);
		}
		return wireFrameSphere;
	}

	Geometry* GeometryFactory::GetCilinder()
	{
		if (!Allocator::IsValid(cilinder)) {
			int resolution = 20;
			std::vector<Vertex_pos_norm_tang_bitang_uv> vertices;
			std::vector<GLuint> indices;

			//Lower circle:
			for (int i = 0; i < resolution; i++) {
				Vertex_pos_norm_tang_bitang_uv v;
				v.position = glm::vec3(cosf((float)i / resolution * 2 * M_PI), -1.0f, sinf((float)i / resolution * 2 * M_PI));
				vertices.push_back(v);
			}
			//Upper circle:
			for (int i = 0; i < resolution; i++) {
				Vertex_pos_norm_tang_bitang_uv v;
				v.position = glm::vec3(cosf((float)i / resolution * 2 * M_PI), 1.0f, sinf((float)i / resolution * 2 * M_PI));
				vertices.push_back(v);
			}
			// Indices:
			for (int i = 0; i < resolution; i++) {
				if (i < resolution - 1) {
					indices.push_back(i);
					indices.push_back(i + resolution);
					indices.push_back(i + 1);

					indices.push_back(i + 1);
					indices.push_back(i + resolution);
					indices.push_back(i + resolution + 1);
				}
				else {
					indices.push_back(i);
					indices.push_back(i + resolution);
					indices.push_back(0);

					indices.push_back(0);
					indices.push_back(resolution);
					indices.push_back(resolution + 1);
				}
			}

			cilinder = Allocator::New<Geometry>();
			cilinder->Init(vertices, indices);
		}
		return cilinder;
	}

	Geometry* GeometryFactory::getLightVolumeSphere()
	{
		if (!Allocator::IsValid(lightVolumeSphere)) {
			lightVolumeSphere = GenerateSphere();
			lightVolumeSphere->SetFaceCulling(true);
			lightVolumeSphere->SetFaceCullingOrientation(GL_CW);
		}
		return lightVolumeSphere;
	}

	void GeometryFactory::ForgetPointers()
	{
		quad = nullptr;
		cube = nullptr;
		wireframeCube = nullptr;
		sphere = nullptr;
		lightVolumeSphere = nullptr;
		wireFrameSphere = nullptr;
		cilinder = nullptr;
	}

	Geometry* GeometryFactory::GenerateSphere()
	{
		// ------------------- ICOSAHEDRON SPHERE -------------------
		float r = 1.0f;
		float a = 3.0f;
		float b = 1.0f;
		float sum = a + b;

		std::vector<Vertex_pos_norm_tang_bitang_uv> vertices;
		glm::vec3 norm = glm::vec3(1.0f, 1.0f, 1.0f);

		std::vector<glm::vec3> positions;

		// base points of rectangles:

		// x rectangle:
		glm::vec3 x1 = glm::vec3(0, sum / 2, a / 2);
		glm::vec3 x2 = glm::vec3(0, -sum / 2, a / 2);
		glm::vec3 x3 = glm::vec3(0, -sum / 2, -a / 2);
		glm::vec3 x4 = glm::vec3(0, sum / 2, -a / 2);

		positions.push_back(x1);
		positions.push_back(x2);
		positions.push_back(x3);
		positions.push_back(x4);

		// y rectangle:
		glm::vec3 y1 = glm::vec3(a / 2, 0, sum / 2);
		glm::vec3 y2 = glm::vec3(a / 2, 0, -sum / 2);
		glm::vec3 y3 = glm::vec3(-a / 2, 0, -sum / 2);
		glm::vec3 y4 = glm::vec3(-a / 2, 0, sum / 2);

		positions.push_back(y1);
		positions.push_back(y2);
		positions.push_back(y3);
		positions.push_back(y4);

		//z rectangle:
		glm::vec3 z1 = glm::vec3(sum / 2, a / 2, 0);
		glm::vec3 z2 = glm::vec3(-sum / 2, a / 2, 0);
		glm::vec3 z3 = glm::vec3(-sum / 2, -a / 2, 0);
		glm::vec3 z4 = glm::vec3(sum / 2, -a / 2, 0);

		positions.push_back(z1);
		positions.push_back(z2);
		positions.push_back(z3);
		positions.push_back(z4);

		std::vector<GLuint> indices;

		int resolution = 5;

		// generates points on sphere from icosaheder faces
		generateIcosaFace(x1, y1, y4, resolution, r, &positions, &indices);
		generateIcosaFace(x2, y1, y4, resolution, r, &positions, &indices);
		generateIcosaFace(x3, y2, y3, resolution, r, &positions, &indices);
		generateIcosaFace(x4, y2, y3, resolution, r, &positions, &indices);

		generateIcosaFace(y1, z1, z4, resolution, r, &positions, &indices);
		generateIcosaFace(y2, z1, z4, resolution, r, &positions, &indices);
		generateIcosaFace(y3, z2, z3, resolution, r, &positions, &indices);
		generateIcosaFace(y4, z2, z3, resolution, r, &positions, &indices);

		generateIcosaFace(z1, x1, x4, resolution, r, &positions, &indices);
		generateIcosaFace(z2, x1, x4, resolution, r, &positions, &indices);
		generateIcosaFace(z3, x2, x3, resolution, r, &positions, &indices);
		generateIcosaFace(z4, x2, x3, resolution, r, &positions, &indices);

		generateIcosaFace(x1, y1, z1, resolution, r, &positions, &indices);
		generateIcosaFace(x4, y2, z1, resolution, r, &positions, &indices);
		generateIcosaFace(x1, y4, z2, resolution, r, &positions, &indices);
		generateIcosaFace(x4, y3, z2, resolution, r, &positions, &indices);

		generateIcosaFace(x2, y1, z4, resolution, r, &positions, &indices);
		generateIcosaFace(x3, y2, z4, resolution, r, &positions, &indices);
		generateIcosaFace(x2, y4, z3, resolution, r, &positions, &indices);
		generateIcosaFace(x3, y3, z3, resolution, r, &positions, &indices);

		for (int i = 0; i < positions.size(); i++)
		{
			Vertex_pos_norm_tang_bitang_uv vert;
			vert.position = glm::vec4(positions.at(i), 1.0f);
			vert.normal = normalize(positions.at(i));
			float d = abs(glm::dot(vert.normal, glm::vec3(0.0f, 1.0f, 0.0f)));
			glm::vec3 up = (d < 0.99999f) ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(0.0f, 0.0f, 1.0f);
			vert.tangent = normalize(glm::cross(up, vert.normal));
			vert.bitangent = normalize(glm::cross(vert.normal, vert.tangent));
			glm::vec2 horizontal = glm::normalize(glm::vec2(vert.position.x, vert.position.z));
			vert.uv = glm::vec2(horizontal.x * 0.5f + 0.5f, vert.normal.y * 0.5f + 0.5f);
			vertices.push_back(vert);
		}
		auto* geom = Allocator::New<Geometry>();
		geom->Init(vertices, indices);
		return geom;
	}

	void GeometryFactory::generateIcosaFace(glm::vec3 a, glm::vec3 b, glm::vec3 c, int resolution, float r, std::vector<glm::vec3>* vertices, std::vector<GLuint>* indices)
	{
		if (0.0 > glm::dot(glm::cross(b - a, c - a), a)) {
			glm::vec3 temp = a;
			a = b;
			b = temp;
		}

		a = normalize(a);
		a = r * a;
		b = normalize(b);
		b = r * b;
		c = normalize(c);
		c = r * c;

		glm::vec3 aToB = (b - a) / (float)(resolution + 1.0f);
		glm::vec3 bToC = (c - b) / (float)(resolution + 1.0f);

		int vertexIndex = vertices->size();
		int startIndex = vertexIndex;

		vertices->push_back(a);

		for (int i = 1; i < resolution + 2; i++)
		{
			glm::vec3 iterationBase = a + ((float)i * aToB);

			for (int j = 0; j < i + 1; j++)
			{
				glm::vec3 currentVector = iterationBase + ((float)j * bToC);
				currentVector = (r / length(currentVector)) * currentVector;
				vertices->push_back(currentVector);
			}
		}

		for (int i = 1; i < resolution + 2; i++)
		{
			for (int j = 0; j < i; j++)
			{
				vertexIndex++;
				indices->push_back(vertexIndex - i);
				indices->push_back(vertexIndex);
				indices->push_back(vertexIndex + 1);

				if (j != 0)
				{
					indices->push_back(vertexIndex - i);
					indices->push_back(vertexIndex - i - 1);
					indices->push_back(vertexIndex);
				}
			}

			vertexIndex++;
		}
	}

GeometryFactory* GeometryFactory::GetInstance()
{
	if (nullptr == instance) {
		instance = Allocator::New<GeometryFactory>();
	}
	return instance;
}

void GeometryFactory::DestroyInstance()
{
	Allocator::Delete(instance);
}

Geometry* GeometryFactory::GetQuad()
{
	if (Allocator::IsValid(quad)) {
		return quad;
	}
	std::vector<Vertex_pos_norm_tang_bitang_uv> vertices;
	std::vector<GLuint> indices;
	/*
	constexpr float quadVertices[] =
	{
		//Coord	//texCoords
		1.0f, -1.0f,  1.0f,  0.0f,
	   -1.0f, -1.0f,  0.0f,  0.0f,
	   -1.0f,  1.0f,  0.0f,  1.0f,

		1.0f,  1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,  0.0f,
	   -1.0f,  1.0f,  0.0f,  1.0f
	};
	*/

	Vertex_pos_norm_tang_bitang_uv v0;
	v0.position = glm::vec3(1.0f, -1.0f, 0.0f);
	v0.uv = glm::vec2(1.0f, 0.0f);
	v0.normal = glm::vec3(0.0f, 0.0f, 1.0f);
	v0.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
	v0.bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
	vertices.push_back(v0);

	Vertex_pos_norm_tang_bitang_uv v1;
	v1.position = glm::vec3(-1.0f, -1.0f, 0.0f);
	v1.uv = glm::vec2(0.0f, 0.0f);
	v1.normal = glm::vec3(0.0f, 0.0f, 1.0f);
	v1.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
	v1.bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
	vertices.push_back(v1);

	Vertex_pos_norm_tang_bitang_uv v2;
	v2.position = glm::vec3(-1.0f, 1.0f, 0.0f);
	v2.uv = glm::vec2(0.0f, 1.0f);
	v2.normal = glm::vec3(0.0f, 0.0f, 1.0f);
	v2.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
	v2.bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
	vertices.push_back(v2);

	Vertex_pos_norm_tang_bitang_uv v3;
	v3.position = glm::vec3(1.0f, 1.0f, 0.0f);
	v3.uv = glm::vec2(1.0f, 1.0f);
	v3.normal = glm::vec3(0.0f, 0.0f, 1.0f);
	v3.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
	v3.bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
	vertices.push_back(v3);

	Vertex_pos_norm_tang_bitang_uv v4;
	v4.position = glm::vec3(1.0f, -1.0f, 0.0f);
	v4.uv = glm::vec2(1.0f, 0.0f);
	v4.normal = glm::vec3(0.0f, 0.0f, 1.0f);
	v4.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
	v4.bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
	vertices.push_back(v4);

	Vertex_pos_norm_tang_bitang_uv v5;
	v5.position = glm::vec3(-1.0f, 1.0f, 0.0f);
	v5.uv = glm::vec2(0.0f, 1.0f);
	v5.normal = glm::vec3(0.0f, 0.0f, 1.0f);
	v5.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
	v5.bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
	vertices.push_back(v5);

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(4);
	indices.push_back(5);
	quad = Allocator::New<Geometry>();
	quad->Init(vertices, indices);
	quad->SetFaceCulling(false);
	return quad;
}

Geometry* GeometryFactory::GetSimpleQuad()
{
	if (Allocator::IsValid(simpleQuad)) {
		return simpleQuad;
	}
	std::vector<Vertex_2d_pos_uv> vertices;
	std::vector<GLuint> indices;

	Vertex_2d_pos_uv v0;
	v0.position = glm::vec2(1.0f, -1.0f);
	v0.uv = glm::vec2(1.0f, 0.0f);
	vertices.push_back(v0);

	Vertex_2d_pos_uv v1;
	v1.position = glm::vec2(-1.0f, -1.0f);
	v1.uv = glm::vec2(0.0f, 0.0f);
	vertices.push_back(v1);

	Vertex_2d_pos_uv v2;
	v2.position = glm::vec2(-1.0f, 1.0f);
	v2.uv = glm::vec2(0.0f, 1.0f);
	vertices.push_back(v2);

	Vertex_2d_pos_uv v3;
	v3.position = glm::vec2(1.0f, 1.0f);
	v3.uv = glm::vec2(1.0f, 1.0f);
	vertices.push_back(v3);

	Vertex_2d_pos_uv v4;
	v4.position = glm::vec2(1.0f, -1.0f);
	v4.uv = glm::vec2(1.0f, 0.0f);
	vertices.push_back(v4);

	Vertex_2d_pos_uv v5;
	v5.position = glm::vec2(-1.0f, 1.0f);
	v5.uv = glm::vec2(0.0f, 1.0f);
	vertices.push_back(v5);

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(4);
	indices.push_back(5);
	simpleQuad = Allocator::New<Geometry>();
	simpleQuad->Init(vertices, indices);
	simpleQuad->SetFaceCulling(false);
	return simpleQuad;
}

Geometry* GeometryFactory::GetCube()
{
	if (Allocator::IsValid(cube)) {
		return cube;
	}
	std::vector<Vertex_pos_norm_tang_bitang_uv> vertices;
	std::vector<GLuint> indices;

	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 bitangent;
	normal = glm::vec3(0.0f, 0.0f, -1.0f);
	tangent = glm::vec3(1.0f, 0.0f, 0.0f);
	bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
	Vertex_pos_norm_tang_bitang_uv v0;
	v0.position = glm::vec3(-1.0f, 1.0f, -1.0f);
	v0.normal = normal;
	v0.tangent = tangent;
	v0.bitangent = bitangent;
	v0.uv = glm::vec2(0.0, 1.0);
	Vertex_pos_norm_tang_bitang_uv v1;
	v1.position = glm::vec3(-1.0f, -1.0f, -1.0f);
	v1.normal = normal;
	v1.tangent = tangent;
	v1.bitangent = bitangent;
	v1.uv = glm::vec2(0.0, 0.0);
	Vertex_pos_norm_tang_bitang_uv v2;
	v2.position = glm::vec3(1.0f, -1.0f, -1.0f);
	v2.normal = normal;
	v2.tangent = tangent;
	v2.bitangent = bitangent;
	v2.uv = glm::vec2(1.0, 0.0);
	vertices.push_back(v2);
	vertices.push_back(v1);
	vertices.push_back(v0);
	Vertex_pos_norm_tang_bitang_uv v3;
	v3.position = glm::vec3(1.0f, -1.0f, -1.0f);
	v3.normal = normal;
	v3.tangent = tangent;
	v3.bitangent = bitangent;
	v3.uv = glm::vec2(1.0, 0.0);
	Vertex_pos_norm_tang_bitang_uv v4;
	v4.position = glm::vec3(1.0f, 1.0f, -1.0f);
	v4.normal = normal;
	v4.tangent = tangent;
	v4.bitangent = bitangent;
	v4.uv = glm::vec2(1.0, 1.0);
	Vertex_pos_norm_tang_bitang_uv v5;
	v5.position = glm::vec3(-1.0f, 1.0f, -1.0f);
	v5.normal = normal;
	v5.tangent = tangent;
	v5.bitangent = bitangent;
	v5.uv = glm::vec2(0.0, 1.0);
	vertices.push_back(v5);
	vertices.push_back(v4);
	vertices.push_back(v3);

	normal = glm::vec3(-1.0f, 0.0f, 0.0f);
	tangent = glm::vec3(0.0f, 0.0f, -1.0f);
	bitangent = glm::vec3(0.0f, 1.0f, 0.0f);

	Vertex_pos_norm_tang_bitang_uv v6;
	v6.position = glm::vec3(-1.0f, -1.0f, 1.0f);
	v6.normal = normal;
	v6.tangent = tangent;
	v6.bitangent = bitangent;
	v6.uv = glm::vec2(0.0, 1.0);
	Vertex_pos_norm_tang_bitang_uv v7;
	v7.position = glm::vec3(-1.0f, -1.0f, -1.0f);
	v7.normal = normal;
	v7.tangent = tangent;
	v7.bitangent = bitangent;
	v7.uv = glm::vec2(0.0, 0.0);
	Vertex_pos_norm_tang_bitang_uv v8;
	v8.position = glm::vec3(-1.0f, 1.0f, -1.0f);
	v8.normal = normal;
	v8.tangent = tangent;
	v8.bitangent = bitangent;
	v8.uv = glm::vec2(1.0, 0.0);
	vertices.push_back(v8);
	vertices.push_back(v7);
	vertices.push_back(v6);
	Vertex_pos_norm_tang_bitang_uv v9;
	v9.position = glm::vec3(-1.0f, 1.0f, -1.0f);
	v9.normal = normal;
	v9.tangent = tangent;
	v9.bitangent = bitangent;
	v9.uv = glm::vec2(1.0, 0.0);
	Vertex_pos_norm_tang_bitang_uv v10;
	v10.position = glm::vec3(-1.0f, 1.0f, 1.0f);
	v10.normal = normal;
	v10.tangent = tangent;
	v10.bitangent = bitangent;
	v10.uv = glm::vec2(1.0, 1.0);
	Vertex_pos_norm_tang_bitang_uv v11;
	v11.position = glm::vec3(-1.0f, -1.0f, 1.0f);
	v11.normal = normal;
	v11.tangent = tangent;
	v11.bitangent = bitangent;
	v11.uv = glm::vec2(0.0, 1.0);
	vertices.push_back(v11);
	vertices.push_back(v10);
	vertices.push_back(v9);

	normal = glm::vec3(1.0f, 0.0f, 0.0f);
	tangent = glm::vec3(0.0f, 0.0f, 1.0f);
	bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
	Vertex_pos_norm_tang_bitang_uv v12;
	v12.position = glm::vec3(1.0f, -1.0f, -1.0f);
	v12.normal = normal;
	v12.tangent = tangent;
	v12.bitangent = bitangent;
	v12.uv = glm::vec2(0.0, 0.0);
	Vertex_pos_norm_tang_bitang_uv v13;
	v13.position = glm::vec3(1.0f, -1.0f, 1.0f);
	v13.normal = normal;
	v13.tangent = tangent;
	v13.bitangent = bitangent;
	v13.uv = glm::vec2(0.0, 1.0);
	Vertex_pos_norm_tang_bitang_uv v14;
	v14.position = glm::vec3(1.0f, 1.0f, 1.0f);
	v14.normal = normal;
	v14.tangent = tangent;
	v14.bitangent = bitangent;
	v14.uv = glm::vec2(1.0, 1.0);
	vertices.push_back(v14);
	vertices.push_back(v13);
	vertices.push_back(v12);
	Vertex_pos_norm_tang_bitang_uv v15;
	v15.position = glm::vec3(1.0f, 1.0f, 1.0f);
	v15.normal = normal;
	v15.tangent = tangent;
	v15.bitangent = bitangent;
	v15.uv = glm::vec2(1.0, 1.0);
	Vertex_pos_norm_tang_bitang_uv v16;
	v16.position = glm::vec3(1.0f, 1.0f, -1.0f);
	v16.normal = normal;
	v16.tangent = tangent;
	v16.bitangent = bitangent;
	v16.uv = glm::vec2(1.0, 0.0);
	Vertex_pos_norm_tang_bitang_uv v17;
	v17.position = glm::vec3(1.0f, -1.0f, -1.0f);
	v17.normal = normal;
	v17.tangent = tangent;
	v17.bitangent = bitangent;
	v17.uv = glm::vec2(0.0, 0.0);
	vertices.push_back(v17);
	vertices.push_back(v16);
	vertices.push_back(v15);

	normal = glm::vec3(0.0f, 0.0f, 1.0f);
	tangent = glm::vec3(-1.0f, 0.0f, 0.0f);
	bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
	Vertex_pos_norm_tang_bitang_uv v18;
	v18.position = glm::vec3(-1.0f, -1.0f, 1.0f);
	v18.normal = normal;
	v18.tangent = tangent;
	v18.bitangent = bitangent;
	v18.uv = glm::vec2(0.0, 0.0);
	Vertex_pos_norm_tang_bitang_uv v19;
	v19.position = glm::vec3(-1.0f, 1.0f, 1.0f);
	v19.normal = normal;
	v19.tangent = tangent;
	v19.bitangent = bitangent;
	v19.uv = glm::vec2(0.0, 1.0);
	Vertex_pos_norm_tang_bitang_uv v20;
	v20.position = glm::vec3(1.0f, 1.0f, 1.0f);
	v20.normal = normal;
	v20.tangent = tangent;
	v20.bitangent = bitangent;
	v20.uv = glm::vec2(1.0, 1.0);
	vertices.push_back(v20);
	vertices.push_back(v19);
	vertices.push_back(v18);
	Vertex_pos_norm_tang_bitang_uv v21;
	v21.position = glm::vec3(1.0f, 1.0f, 1.0f);
	v21.normal = normal;
	v21.tangent = tangent;
	v21.bitangent = bitangent;
	v21.uv = glm::vec2(1.0, 1.0);
	Vertex_pos_norm_tang_bitang_uv v22;
	v22.position = glm::vec3(1.0f, -1.0f, 1.0f);
	v22.normal = normal;
	v22.tangent = tangent;
	v22.bitangent = bitangent;
	v22.uv = glm::vec2(1.0, 0.0);
	Vertex_pos_norm_tang_bitang_uv v23;
	v23.position = glm::vec3(-1.0f, -1.0f, 1.0f);
	v23.normal = normal;
	v23.tangent = tangent;
	v23.bitangent = bitangent;
	v23.uv = glm::vec2(0.0, 0.0);
	vertices.push_back(v23);
	vertices.push_back(v22);
	vertices.push_back(v21);

	normal = glm::vec3(0.0f, 1.0f, 0.0f);
	tangent = glm::vec3(1.0f, 0.0f, 0.0f);
	bitangent = glm::vec3(0.0f, 0.0f, 1.0f);
	Vertex_pos_norm_tang_bitang_uv v24;
	v24.position = glm::vec3(-1.0f, 1.0f, -1.0f);
	v24.normal = normal;
	v24.tangent = tangent;
	v24.bitangent = bitangent;
	v24.uv = glm::vec2(0.0, 0.0);
	Vertex_pos_norm_tang_bitang_uv v25;
	v25.position = glm::vec3(1.0f, 1.0f, -1.0f);
	v25.normal = normal;
	v25.tangent = tangent;
	v25.bitangent = bitangent;
	v25.uv = glm::vec2(1.0, 0.0);
	Vertex_pos_norm_tang_bitang_uv v26;
	v26.position = glm::vec3(1.0f, 1.0f, 1.0f);
	v26.normal = normal;
	v26.tangent = tangent;
	v26.bitangent = bitangent;
	v26.uv = glm::vec2(1.0, 1.0);
	vertices.push_back(v26);
	vertices.push_back(v25);
	vertices.push_back(v24);
	Vertex_pos_norm_tang_bitang_uv v27;
	v27.position = glm::vec3(1.0f, 1.0f, 1.0f);
	v27.normal = normal;
	v27.tangent = tangent;
	v27.bitangent = bitangent;
	v27.uv = glm::vec2(1.0, 1.0);
	Vertex_pos_norm_tang_bitang_uv v28;
	v28.position = glm::vec3(-1.0f, 1.0f, 1.0f);
	v28.normal = normal;
	v28.tangent = tangent;
	v28.bitangent = bitangent;
	v28.uv = glm::vec2(0.0, 1.0);
	Vertex_pos_norm_tang_bitang_uv v29;
	v29.position = glm::vec3(-1.0f, 1.0f, -1.0f);
	v29.normal = normal;
	v29.tangent = tangent;
	v29.bitangent = bitangent;
	v29.uv = glm::vec2(0.0, 0.0);
	vertices.push_back(v29);
	vertices.push_back(v28);
	vertices.push_back(v27);

	normal = glm::vec3(0.0f, -1.0f, 0.0f);
	tangent = glm::vec3(1.0f, 0.0f, 0.0f);
	bitangent = glm::vec3(0.0f, 0.0f, -1.0f);
	Vertex_pos_norm_tang_bitang_uv v30;
	v30.position = glm::vec3(-1.0f, -1.0f, -1.0f);
	v30.normal = normal;
	v30.tangent = tangent;
	v30.bitangent = bitangent;
	v30.uv = glm::vec2(0.0, 0.0);
	Vertex_pos_norm_tang_bitang_uv v31;
	v31.position = glm::vec3(-1.0f, -1.0f, 1.0f);
	v31.normal = normal;
	v31.tangent = tangent;
	v31.bitangent = bitangent;
	v31.uv = glm::vec2(0.0, 1.0);
	Vertex_pos_norm_tang_bitang_uv v32;
	v32.position = glm::vec3(1.0f, -1.0f, -1.0f);
	v32.normal = normal;
	v32.tangent = tangent;
	v32.bitangent = bitangent;
	v32.uv = glm::vec2(1.0, 0.0);
	vertices.push_back(v32);
	vertices.push_back(v31);
	vertices.push_back(v30);
	Vertex_pos_norm_tang_bitang_uv v33;
	v33.position = glm::vec3(1.0f, -1.0f, -1.0f);
	v33.normal = normal;
	v33.tangent = tangent;
	v33.bitangent = bitangent;
	v33.uv = glm::vec2(1.0, 0.0);
	Vertex_pos_norm_tang_bitang_uv v34;
	v34.position = glm::vec3(-1.0f, -1.0f, 1.0f);
	v34.normal = normal;
	v34.tangent = tangent;
	v34.bitangent = bitangent;
	v34.uv = glm::vec2(0.0, 0.0);
	Vertex_pos_norm_tang_bitang_uv v35;
	v35.position = glm::vec3(1.0f, -1.0f, 1.0f);
	v35.normal = normal;
	v35.tangent = tangent;
	v35.bitangent = bitangent;
	v35.uv = glm::vec2(1.0, 1.0);
	vertices.push_back(v35);
	vertices.push_back(v34);
	vertices.push_back(v33);

	for (int i = 0; i < 36; i++) {
		indices.push_back(i);
	}
	cube = Allocator::New<Geometry>();
	cube->Init(vertices, indices);
	return cube;
}

Geometry* GeometryFactory::GetWireframeCube()
{
	if (Allocator::IsValid(wireframeCube)) {
		return wireframeCube;
	}
	std::vector<Vertex_pos_norm_tang_bitang_uv> vertices;
	std::vector<GLuint> indices;
	Vertex_pos_norm_tang_bitang_uv v;
	v.position = glm::vec3(-1, -1, -1);
	vertices.push_back(v);
	v.position = glm::vec3(1, -1, -1);
	vertices.push_back(v);
	v.position = glm::vec3(1, -1, 1);
	vertices.push_back(v);
	v.position = glm::vec3(-1, -1, 1);
	vertices.push_back(v);

	v.position = glm::vec3(-1, 1, -1);
	vertices.push_back(v);
	v.position = glm::vec3(1, 1, -1);
	vertices.push_back(v);
	v.position = glm::vec3(1, 1, 1);
	vertices.push_back(v);
	v.position = glm::vec3(-1, 1, 1);
	vertices.push_back(v);

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(3);
	indices.push_back(0);

	indices.push_back(4);
	indices.push_back(5);
	indices.push_back(5);
	indices.push_back(6);
	indices.push_back(6);
	indices.push_back(7);
	indices.push_back(7);
	indices.push_back(4);

	indices.push_back(0);
	indices.push_back(4);
	indices.push_back(1);
	indices.push_back(5);
	indices.push_back(2);
	indices.push_back(6);
	indices.push_back(3);
	indices.push_back(7);

	wireframeCube = Allocator::New<Geometry>();
	wireframeCube->Init(vertices, indices);
	wireframeCube->SetPrimitiveType(GL_LINES);
	wireframeCube->SetFaceCulling(false);
	return wireframeCube;
}

}
