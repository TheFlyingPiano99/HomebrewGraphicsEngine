#include "GeometryFactory.h"
#include <complex>
#include "GlobalInclude.h"

namespace Hogra {

	GeometryFactory* GeometryFactory::instance = nullptr;

	Geometry* GeometryFactory::getSphere()
	{
		if (nullptr == sphere) {
			sphere = generateSphere();
			sphere->SetFaceCulling(true);
		}
		return sphere;
	}

	Geometry* GeometryFactory::getWireFrameSphere()
	{
		if (nullptr == wireFrameSphere) {
			std::vector<Vertex> vertices;
			std::vector<int> indices;
			int zRes = 100;
			int yRes = 100;
			int xRes = 100;
			float r = 1.0f;
			for (int i = 0; i < zRes; i++) {
				Vertex v;
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
				Vertex v;
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
				Vertex v;
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
			wireFrameSphere = new Geometry(vertices, indices);
			wireFrameSphere->SetPrimitiveType(GL_LINES);
			wireFrameSphere->SetFaceCulling(false);
		}
		return wireFrameSphere;
	}

	Geometry* GeometryFactory::getLightVolumeSphere()
	{
		if (nullptr == lightVolumeSphere) {
			lightVolumeSphere = generateSphere();
			lightVolumeSphere->SetFaceCulling(true);
			lightVolumeSphere->SetFaceCullingOrientation(GL_CW);
		}
		return lightVolumeSphere;
	}

	Geometry* GeometryFactory::generateSphere()
	{
		// ------------------- ICOSAHEDRON SPHERE -------------------
		float r = 1.0f;
		float a = 3.0f;
		float b = 1.0f;
		float sum = a + b;

		std::vector<Vertex> vertices;
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

		std::vector<GLint> indices;

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
			Vertex vert;
			vert.position = glm::vec4(positions.at(i), 1.0f);
			vert.normal = normalize(positions.at(i));
			vert.tangent = normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), vert.normal));
			vert.bitangent = normalize(glm::cross(vert.normal, vert.tangent));
			glm::vec2 horizontal = glm::normalize(glm::vec2(vert.position.x, vert.position.z));
			vert.texUV = glm::vec2(horizontal.x * 0.5f + 0.5f, vert.normal.y * 0.5f + 0.5f);
			vertices.push_back(vert);
		}

		return new Geometry(vertices, indices);
	}

	void GeometryFactory::generateIcosaFace(glm::vec3 a, glm::vec3 b, glm::vec3 c, int resolution, float r, std::vector<glm::vec3>* vertices, std::vector<GLint>* indices)
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

GeometryFactory* GeometryFactory::getInstance()
{
	if (nullptr == instance) {
		instance = new GeometryFactory();
	}
	return instance;
}

void GeometryFactory::DestroyInstance()
{
	if (nullptr != instance) {
		delete instance;
	}
	instance = nullptr;
}

Geometry* GeometryFactory::getFullScreenQuad()
{
	if (nullptr != fullScreenQuad) {
		return fullScreenQuad;
	}
	std::vector<Vertex> vertices;
	std::vector<GLint> indices;
	float quadVertices[] =
	{
		//Coord	//texCoords
		1.0f, -1.0f,  1.0f,  0.0f,
	   -1.0f, -1.0f,  0.0f,  0.0f,
	   -1.0f,  1.0f,  0.0f,  1.0f,

		1.0f,  1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,  0.0f,
	   -1.0f,  1.0f,  0.0f,  1.0f
	};
	Vertex v0;
	v0.position = glm::vec3(1.0f, -1.0f, 0.0f);
	v0.texUV = glm::vec2(1.0f, 0.0f);
	v0.normal = glm::vec3(0.0f, 0.0f, 1.0f);
	v0.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
	v0.bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
	vertices.push_back(v0);

	Vertex v1;
	v1.position = glm::vec3(-1.0f, -1.0f, 0.0f);
	v1.texUV = glm::vec2(0.0f, 0.0f);
	v1.normal = glm::vec3(0.0f, 0.0f, 1.0f);
	v1.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
	v1.bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
	vertices.push_back(v1);

	Vertex v2;
	v2.position = glm::vec3(-1.0f, 1.0f, 0.0f);
	v2.texUV = glm::vec2(0.0f, 1.0f);
	v2.normal = glm::vec3(0.0f, 0.0f, 1.0f);
	v2.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
	v2.bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
	vertices.push_back(v2);

	Vertex v3;
	v3.position = glm::vec3(1.0f, 1.0f, 0.0f);
	v3.texUV = glm::vec2(1.0f, 1.0f);
	v3.normal = glm::vec3(0.0f, 0.0f, 1.0f);
	v3.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
	v3.bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
	vertices.push_back(v3);

	Vertex v4;
	v4.position = glm::vec3(1.0f, -1.0f, 0.0f);
	v4.texUV = glm::vec2(1.0f, 0.0f);
	v4.normal = glm::vec3(0.0f, 0.0f, 1.0f);
	v4.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
	v4.bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
	vertices.push_back(v4);

	Vertex v5;
	v5.position = glm::vec3(-1.0f, 1.0f, 0.0f);
	v5.texUV = glm::vec2(0.0f, 1.0f);
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
	fullScreenQuad = new Geometry(vertices, indices);
	fullScreenQuad->SetFaceCulling(false);
	return fullScreenQuad;
}

Geometry* GeometryFactory::getCube()
{
	if (nullptr != cube) {
		return cube;
	}
	std::vector<Vertex> vertices;
	std::vector<GLint> indices;

	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 bitangent;
	normal = glm::vec3(0.0f, 0.0f, -1.0f);
	tangent = glm::vec3(1.0f, 0.0f, 0.0f);
	bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
	Vertex v0;
	v0.position = glm::vec3(-1.0f, 1.0f, -1.0f);
	v0.normal = normal;
	v0.tangent = tangent;
	v0.bitangent = bitangent;
	v0.texUV = glm::vec2(0.0, 1.0);
	Vertex v1;
	v1.position = glm::vec3(-1.0f, -1.0f, -1.0f);
	v1.normal = normal;
	v1.tangent = tangent;
	v1.bitangent = bitangent;
	v1.texUV = glm::vec2(0.0, 0.0);
	Vertex v2;
	v2.position = glm::vec3(1.0f, -1.0f, -1.0f);
	v2.normal = normal;
	v2.tangent = tangent;
	v2.bitangent = bitangent;
	v2.texUV = glm::vec2(1.0, 0.0);
	vertices.push_back(v2);
	vertices.push_back(v1);
	vertices.push_back(v0);
	Vertex v3;
	v3.position = glm::vec3(1.0f, -1.0f, -1.0f);
	v3.normal = normal;
	v3.tangent = tangent;
	v3.bitangent = bitangent;
	v3.texUV = glm::vec2(1.0, 0.0);
	Vertex v4;
	v4.position = glm::vec3(1.0f, 1.0f, -1.0f);
	v4.normal = normal;
	v4.tangent = tangent;
	v4.bitangent = bitangent;
	v4.texUV = glm::vec2(1.0, 1.0);
	Vertex v5;
	v5.position = glm::vec3(-1.0f, 1.0f, -1.0f);
	v5.normal = normal;
	v5.tangent = tangent;
	v5.bitangent = bitangent;
	v5.texUV = glm::vec2(0.0, 1.0);
	vertices.push_back(v5);
	vertices.push_back(v4);
	vertices.push_back(v3);

	normal = glm::vec3(-1.0f, 0.0f, 0.0f);
	tangent = glm::vec3(0.0f, 0.0f, -1.0f);
	bitangent = glm::vec3(0.0f, 1.0f, 0.0f);

	Vertex v6;
	v6.position = glm::vec3(-1.0f, -1.0f, 1.0f);
	v6.normal = normal;
	v6.tangent = tangent;
	v6.bitangent = bitangent;
	v6.texUV = glm::vec2(0.0, 1.0);
	Vertex v7;
	v7.position = glm::vec3(-1.0f, -1.0f, -1.0f);
	v7.normal = normal;
	v7.tangent = tangent;
	v7.bitangent = bitangent;
	v7.texUV = glm::vec2(0.0, 0.0);
	Vertex v8;
	v8.position = glm::vec3(-1.0f, 1.0f, -1.0f);
	v8.normal = normal;
	v8.tangent = tangent;
	v8.bitangent = bitangent;
	v8.texUV = glm::vec2(1.0, 0.0);
	vertices.push_back(v8);
	vertices.push_back(v7);
	vertices.push_back(v6);
	Vertex v9;
	v9.position = glm::vec3(-1.0f, 1.0f, -1.0f);
	v9.normal = normal;
	v9.tangent = tangent;
	v9.bitangent = bitangent;
	v9.texUV = glm::vec2(1.0, 0.0);
	Vertex v10;
	v10.position = glm::vec3(-1.0f, 1.0f, 1.0f);
	v10.normal = normal;
	v10.tangent = tangent;
	v10.bitangent = bitangent;
	v10.texUV = glm::vec2(1.0, 1.0);
	Vertex v11;
	v11.position = glm::vec3(-1.0f, -1.0f, 1.0f);
	v11.normal = normal;
	v11.tangent = tangent;
	v11.bitangent = bitangent;
	v11.texUV = glm::vec2(0.0, 1.0);
	vertices.push_back(v11);
	vertices.push_back(v10);
	vertices.push_back(v9);

	normal = glm::vec3(1.0f, 0.0f, 0.0f);
	tangent = glm::vec3(0.0f, 0.0f, 1.0f);
	bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
	Vertex v12;
	v12.position = glm::vec3(1.0f, -1.0f, -1.0f);
	v12.normal = normal;
	v12.tangent = tangent;
	v12.bitangent = bitangent;
	v12.texUV = glm::vec2(0.0, 0.0);
	Vertex v13;
	v13.position = glm::vec3(1.0f, -1.0f, 1.0f);
	v13.normal = normal;
	v13.tangent = tangent;
	v13.bitangent = bitangent;
	v13.texUV = glm::vec2(0.0, 1.0);
	Vertex v14;
	v14.position = glm::vec3(1.0f, 1.0f, 1.0f);
	v14.normal = normal;
	v14.tangent = tangent;
	v14.bitangent = bitangent;
	v14.texUV = glm::vec2(1.0, 1.0);
	vertices.push_back(v14);
	vertices.push_back(v13);
	vertices.push_back(v12);
	Vertex v15;
	v15.position = glm::vec3(1.0f, 1.0f, 1.0f);
	v15.normal = normal;
	v15.tangent = tangent;
	v15.bitangent = bitangent;
	v15.texUV = glm::vec2(1.0, 1.0);
	Vertex v16;
	v16.position = glm::vec3(1.0f, 1.0f, -1.0f);
	v16.normal = normal;
	v16.tangent = tangent;
	v16.bitangent = bitangent;
	v16.texUV = glm::vec2(1.0, 0.0);
	Vertex v17;
	v17.position = glm::vec3(1.0f, -1.0f, -1.0f);
	v17.normal = normal;
	v17.tangent = tangent;
	v17.bitangent = bitangent;
	v17.texUV = glm::vec2(0.0, 0.0);
	vertices.push_back(v17);
	vertices.push_back(v16);
	vertices.push_back(v15);

	normal = glm::vec3(0.0f, 0.0f, 1.0f);
	tangent = glm::vec3(-1.0f, 0.0f, 0.0f);
	bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
	Vertex v18;
	v18.position = glm::vec3(-1.0f, -1.0f, 1.0f);
	v18.normal = normal;
	v18.tangent = tangent;
	v18.bitangent = bitangent;
	v18.texUV = glm::vec2(0.0, 0.0);
	Vertex v19;
	v19.position = glm::vec3(-1.0f, 1.0f, 1.0f);
	v19.normal = normal;
	v19.tangent = tangent;
	v19.bitangent = bitangent;
	v19.texUV = glm::vec2(0.0, 1.0);
	Vertex v20;
	v20.position = glm::vec3(1.0f, 1.0f, 1.0f);
	v20.normal = normal;
	v20.tangent = tangent;
	v20.bitangent = bitangent;
	v20.texUV = glm::vec2(1.0, 1.0);
	vertices.push_back(v20);
	vertices.push_back(v19);
	vertices.push_back(v18);
	Vertex v21;
	v21.position = glm::vec3(1.0f, 1.0f, 1.0f);
	v21.normal = normal;
	v21.tangent = tangent;
	v21.bitangent = bitangent;
	v21.texUV = glm::vec2(1.0, 1.0);
	Vertex v22;
	v22.position = glm::vec3(1.0f, -1.0f, 1.0f);
	v22.normal = normal;
	v22.tangent = tangent;
	v22.bitangent = bitangent;
	v22.texUV = glm::vec2(1.0, 0.0);
	Vertex v23;
	v23.position = glm::vec3(-1.0f, -1.0f, 1.0f);
	v23.normal = normal;
	v23.tangent = tangent;
	v23.bitangent = bitangent;
	v23.texUV = glm::vec2(0.0, 0.0);
	vertices.push_back(v23);
	vertices.push_back(v22);
	vertices.push_back(v21);

	normal = glm::vec3(0.0f, 1.0f, 0.0f);
	tangent = glm::vec3(1.0f, 0.0f, 0.0f);
	bitangent = glm::vec3(0.0f, 0.0f, 1.0f);
	Vertex v24;
	v24.position = glm::vec3(-1.0f, 1.0f, -1.0f);
	v24.normal = normal;
	v24.tangent = tangent;
	v24.bitangent = bitangent;
	v24.texUV = glm::vec2(0.0, 0.0);
	Vertex v25;
	v25.position = glm::vec3(1.0f, 1.0f, -1.0f);
	v25.normal = normal;
	v25.tangent = tangent;
	v25.bitangent = bitangent;
	v25.texUV = glm::vec2(1.0, 0.0);
	Vertex v26;
	v26.position = glm::vec3(1.0f, 1.0f, 1.0f);
	v26.normal = normal;
	v26.tangent = tangent;
	v26.bitangent = bitangent;
	v26.texUV = glm::vec2(1.0, 1.0);
	vertices.push_back(v26);
	vertices.push_back(v25);
	vertices.push_back(v24);
	Vertex v27;
	v27.position = glm::vec3(1.0f, 1.0f, 1.0f);
	v27.normal = normal;
	v27.tangent = tangent;
	v27.bitangent = bitangent;
	v27.texUV = glm::vec2(1.0, 1.0);
	Vertex v28;
	v28.position = glm::vec3(-1.0f, 1.0f, 1.0f);
	v28.normal = normal;
	v28.tangent = tangent;
	v28.bitangent = bitangent;
	v28.texUV = glm::vec2(0.0, 1.0);
	Vertex v29;
	v29.position = glm::vec3(-1.0f, 1.0f, -1.0f);
	v29.normal = normal;
	v29.tangent = tangent;
	v29.bitangent = bitangent;
	v29.texUV = glm::vec2(0.0, 0.0);
	vertices.push_back(v29);
	vertices.push_back(v28);
	vertices.push_back(v27);

	normal = glm::vec3(0.0f, -1.0f, 0.0f);
	tangent = glm::vec3(1.0f, 0.0f, 0.0f);
	bitangent = glm::vec3(0.0f, 0.0f, -1.0f);
	Vertex v30;
	v30.position = glm::vec3(-1.0f, -1.0f, -1.0f);
	v30.normal = normal;
	v30.tangent = tangent;
	v30.bitangent = bitangent;
	v30.texUV = glm::vec2(0.0, 0.0);
	Vertex v31;
	v31.position = glm::vec3(-1.0f, -1.0f, 1.0f);
	v31.normal = normal;
	v31.tangent = tangent;
	v31.bitangent = bitangent;
	v31.texUV = glm::vec2(0.0, 1.0);
	Vertex v32;
	v32.position = glm::vec3(1.0f, -1.0f, -1.0f);
	v32.normal = normal;
	v32.tangent = tangent;
	v32.bitangent = bitangent;
	v32.texUV = glm::vec2(1.0, 0.0);
	vertices.push_back(v32);
	vertices.push_back(v31);
	vertices.push_back(v30);
	Vertex v33;
	v33.position = glm::vec3(1.0f, -1.0f, -1.0f);
	v33.normal = normal;
	v33.tangent = tangent;
	v33.bitangent = bitangent;
	v33.texUV = glm::vec2(1.0, 0.0);
	Vertex v34;
	v34.position = glm::vec3(-1.0f, -1.0f, 1.0f);
	v34.normal = normal;
	v34.tangent = tangent;
	v34.bitangent = bitangent;
	v34.texUV = glm::vec2(0.0, 0.0);
	Vertex v35;
	v35.position = glm::vec3(1.0f, -1.0f, 1.0f);
	v35.normal = normal;
	v35.tangent = tangent;
	v35.bitangent = bitangent;
	v35.texUV = glm::vec2(1.0, 1.0);
	vertices.push_back(v35);
	vertices.push_back(v34);
	vertices.push_back(v33);

	for (int i = 0; i < 36; i++) {
		indices.push_back(i);
	}
	cube = new Geometry(vertices, indices);
	return cube;
}

Geometry* GeometryFactory::getWireframeCube()
{
	if (nullptr != wireframeCube) {
		return wireframeCube;
	}
	std::vector<Vertex> vertices;
	std::vector<int> indices;
	Vertex v;
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

	wireframeCube = new Geometry(vertices, indices);
	wireframeCube->SetPrimitiveType(GL_LINES);
	wireframeCube->SetFaceCulling(false);
	return wireframeCube;
}

}
