#include "GeometryFactory.h"
namespace hograengine {

	namespace GeometryFactory {

		Geometry* FullScreenQuad::geometry = nullptr;

		Geometry* FullScreenQuad::getInstance() {
			if (nullptr == geometry) {
				geometry = create();
			}
			return geometry;
		}

		Geometry* FullScreenQuad::create()
		{
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
			Geometry* geom = new Geometry(vertices, indices);
			geom->setFaceCulling(false);
			return geom;
		}

		void FullScreenQuad::destroyInstance()
		{
			if (geometry != nullptr) {
				delete geometry;
			}
		}

		//---------------------------------------------------

		Geometry* Cube::geometry = nullptr;

		Geometry* Cube::getInstance() {
			if (nullptr == geometry) {
				geometry = create();
			}
			return geometry;
		}

		void Cube::destroyInstance()
		{
			if (geometry != nullptr) {
				delete geometry;
			}
		}

		Geometry* Cube::create()
		{
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
			auto* geom = new Geometry(vertices, indices);
			return geom;
		}
	}
}