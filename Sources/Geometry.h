#pragma once
#include <vector>
#include"VAO.h"
#include"EBO.h"
#include "MemoryManager.h"
#include "Identifiable.h"

namespace Hogra {

	class Geometry : public Identifiable
	{
		friend class Allocator;
	public:

		template<typename VertexType>
		void Init(std::vector <VertexType>& vertices, std::vector <GLuint>& indices);

		template<typename VertexType>
		void Update(std::vector <VertexType>& vertices, std::vector <GLuint>& indices);

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
			glm::vec4 position;
			glm::vec4 powerDensity;
			GLfloat shadowCasterIdx;
		};

		void DrawInstanced(const std::vector<InstanceData>& instanceData, int visibleCount);

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
		void InitInstancedBuffer();
		void InitLightInstancedBuffer();

		unsigned int instancedBuffer = 0;
		std::vector <GLuint> indices;
		VAO vao;
		VBO vbo;

		bool faceCulling = true;

		inline void* operator new(std::size_t size) { return ::operator new(size); }
	};

}