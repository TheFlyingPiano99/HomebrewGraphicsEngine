#pragma once
#include <vector>
#include"VAO.h"
#include"EBO.h"
namespace Hogra {

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

		struct LightInstancedData {
			glm::mat4 modelMatrix;
			glm::vec4 lightPosition;
			glm::vec4 lightPowerDensity;
		};

		void DrawInstanced(const std::vector<InstanceData>& instanceData);

		void DrawInstanced(const std::vector<LightInstancedData>& instanceData);

		void SetFaceCulling(bool cull);

		void SetFaceCullingOrientation(int orientation);

		void BindVAO() {
			vao.Bind();
		}

		void SetPrimitiveType(int type) {
			primitiveType = type;
		}

	private:
		int primitiveType = GL_TRIANGLES;
		int faceCullingOrietation = GL_CCW;
		void initInstancedBuffer();
		void initLightInstancedBuffer();

		unsigned int instancedBuffer = 0;
		std::vector <Vertex> vertices;
		std::vector <GLint> indices;
		VAO vao;
		VBO vbo;

		bool faceCulling = true;
	};

}