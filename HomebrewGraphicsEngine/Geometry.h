#pragma once
#include <vector>
#include"VAO.h"
#include"EBO.h"
namespace hograengine {

	class Geometry
	{
	public:
		Geometry(std::vector <Vertex>& vertices, std::vector <GLint>& indices);

		~Geometry() {
			if (0 != instancedBuffer) {
				glDeleteBuffers(1, &instancedBuffer);
			}
		}

		// Bind the VAO to the currently active GPU program
		void Draw();

		struct InstanceData {
			glm::mat4 modelMatrix;
			glm::mat4 invModelMatrix;
		};

		void DrawInstanced(const std::vector<InstanceData>& instanceData);

		void setFaceCulling(bool cull);

		void BindVAO() {
			VAO.Bind();
		}

		void setPrimitiveType(int type) {
			primitiveType = type;
		}

	private:
		int primitiveType = GL_TRIANGLES;
		void initInstancedBuffer();

		unsigned int instancedBuffer = 0;
		std::vector <Vertex> vertices;
		std::vector <GLint> indices;
		VAO VAO;

		bool faceCulling = true;
	};

}