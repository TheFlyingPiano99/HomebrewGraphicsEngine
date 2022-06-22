#include "GeometryFactory.h"
#include <complex>

namespace hograengine {

	Geometry* GeometryFactory::Sphere::geometry = nullptr;

	Geometry* hograengine::GeometryFactory::Sphere::getInstance()
	{
		if (geometry == nullptr) {
			geometry = create();
		}
		return geometry;
	}

	void hograengine::GeometryFactory::Sphere::destroyInstance()
	{
		if (geometry != nullptr) {
			delete geometry;
		}
	}

	Geometry* hograengine::GeometryFactory::Sphere::create()
	{
		// ------------------- ICOSAHEDRON SPHERE -------------------
		float r = 1.0f;
		float a = 3.0f;
		float b = 1.0f;
		float sum = a + b;

		std::vector<Vertex> vertices;
		glm::vec3 norm = glm::vec3(1.0f, 1.0f, 1.0f);

		std::vector<glm::vec3> positions;

		// base poits of rectangles:

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

		int resolution = 10;

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
			vert.texUV = glm::vec2(acosf(horizontal.x) * 0.5f + 0.5f, asinf(vert.normal.y) * 0.5f + 0.5f);
			vertices.push_back(vert);
		}

		return new Geometry(vertices, indices);
	}

	void hograengine::GeometryFactory::Sphere::generateIcosaFace(glm::vec3 a, glm::vec3 b, glm::vec3 c, int resolution, float r, std::vector<glm::vec3>* vertices, std::vector<GLint>* indices)
	{
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
}
