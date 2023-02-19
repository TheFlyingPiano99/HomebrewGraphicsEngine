#include "Geometry.h"
#include "GlobalInclude.h"
#include "MemoryManager.h"
#include <iostream>

namespace Hogra {

	template<>
	void Geometry::Init(std::vector<Vertex_pos_norm_tang_bitang_uv>& _vertices, std::vector<GLuint>& _indices)
	{
		this->indices = _indices;

		vao.Init();
		vao.Bind();
		// Generates Vertex Buffer Object and links it to vertices
		vbo.Init(_vertices);
		// Generates Element Buffer Object and links it to indices
		EBO ebo;
		ebo.Init(indices);
		// Links VBO attributes such as coordinates and colors to VAO
		vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex_pos_norm_tang_bitang_uv), (void*)0);	// pos
		vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex_pos_norm_tang_bitang_uv), (void*)(3 * sizeof(float)));	// normal
		vao.LinkAttrib(vbo, 2, 3, GL_FLOAT, sizeof(Vertex_pos_norm_tang_bitang_uv), (void*)(6 * sizeof(float)));	// tangent
		vao.LinkAttrib(vbo, 3, 3, GL_FLOAT, sizeof(Vertex_pos_norm_tang_bitang_uv), (void*)(9 * sizeof(float)));	// bitangent
		vao.LinkAttrib(vbo, 4, 2, GL_FLOAT, sizeof(Vertex_pos_norm_tang_bitang_uv), (void*)(12 * sizeof(float)));	// uv
		// Unbind all to prevent accidentally modifying them
		vao.Unbind();
		vbo.Unbind();
		ebo.Unbind();
	}

	template<>
	void Geometry::Init(std::vector<Vertex_pos_norm>& _vertices, std::vector<GLuint>& _indices)
	{
		this->indices = _indices;

		vao.Init();
		vao.Bind();
		// Generates Vertex Buffer Object and links it to vertices
		vbo.Init(_vertices);
		// Generates Element Buffer Object and links it to indices
		EBO ebo;
		ebo.Init(indices);
		// Links VBO attributes such as coordinates and colors to VAO
		vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex_pos_norm), (void*)0);	// pos
		vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex_pos_norm), (void*)(3 * sizeof(float)));	// normal
		// Unbind all to prevent accidentally modifying them
		vao.Unbind();
		vbo.Unbind();
		ebo.Unbind();
	}

	template<>
	void Geometry::Init(std::vector<Vertex_2d_pos_uv>& _vertices, std::vector<GLuint>& _indices)
	{
		this->indices = _indices;

		vao.Init();
		vao.Bind();
		// Generates Vertex Buffer Object and links it to vertices
		vbo.Init(_vertices);
		// Generates Element Buffer Object and links it to indices
		EBO ebo;
		ebo.Init(indices);
		// Links VBO attributes such as coordinates and colors to VAO
		vao.LinkAttrib(vbo, 0, 2, GL_FLOAT, sizeof(Vertex_2d_pos_uv), (void*)0);	// pos
		vao.LinkAttrib(vbo, 1, 2, GL_FLOAT, sizeof(Vertex_2d_pos_uv), (void*)(2 * sizeof(float)));	// uv
		// Unbind all to prevent accidentally modifying them
		vao.Unbind();
		vbo.Unbind();
		ebo.Unbind();
	}

	template<typename VertexType>
	void Geometry::Update(std::vector<VertexType>& _vertices, std::vector<GLuint>& _indices)
	{
		if (0 != instancedBuffer) {
			glDeleteBuffers(1, &instancedBuffer);
			instancedBuffer = 0;
		}
		Init(_vertices, _indices);
	}

	void Geometry::InitInstancedBuffer() {
		glGenBuffers(1, &instancedBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, instancedBuffer);
		std::size_t vec4Size = sizeof(glm::vec4);
		std::size_t instanceDataSize = sizeof(InstanceData);
		glEnableVertexAttribArray(INSTANCED_MODEL_MATRIX_LOCATION + 0);
		glEnableVertexAttribArray(INSTANCED_MODEL_MATRIX_LOCATION + 1);
		glEnableVertexAttribArray(INSTANCED_MODEL_MATRIX_LOCATION + 2);
		glEnableVertexAttribArray(INSTANCED_MODEL_MATRIX_LOCATION + 3);

		glVertexAttribPointer(INSTANCED_MODEL_MATRIX_LOCATION + 0, 4, GL_FLOAT, GL_FALSE, instanceDataSize, (void*)0);
		glVertexAttribPointer(INSTANCED_MODEL_MATRIX_LOCATION + 1, 4, GL_FLOAT, GL_FALSE, instanceDataSize, (void*)(1 * vec4Size));
		glVertexAttribPointer(INSTANCED_MODEL_MATRIX_LOCATION + 2, 4, GL_FLOAT, GL_FALSE, instanceDataSize, (void*)(2 * vec4Size));
		glVertexAttribPointer(INSTANCED_MODEL_MATRIX_LOCATION + 3, 4, GL_FLOAT, GL_FALSE, instanceDataSize, (void*)(3 * vec4Size));

		glVertexAttribDivisor(INSTANCED_MODEL_MATRIX_LOCATION + 0, 1);
		glVertexAttribDivisor(INSTANCED_MODEL_MATRIX_LOCATION + 1, 1);
		glVertexAttribDivisor(INSTANCED_MODEL_MATRIX_LOCATION + 2, 1);
		glVertexAttribDivisor(INSTANCED_MODEL_MATRIX_LOCATION + 3, 1);

		glEnableVertexAttribArray(INSTANCED_INV_MODEL_MATRIX_LOCATION + 0);
		glEnableVertexAttribArray(INSTANCED_INV_MODEL_MATRIX_LOCATION + 1);
		glEnableVertexAttribArray(INSTANCED_INV_MODEL_MATRIX_LOCATION + 2);
		glEnableVertexAttribArray(INSTANCED_INV_MODEL_MATRIX_LOCATION + 3);

		glVertexAttribPointer(INSTANCED_INV_MODEL_MATRIX_LOCATION + 0, 4, GL_FLOAT, GL_FALSE, instanceDataSize, (void*)(4 * vec4Size));
		glVertexAttribPointer(INSTANCED_INV_MODEL_MATRIX_LOCATION + 1, 4, GL_FLOAT, GL_FALSE, instanceDataSize, (void*)(5 * vec4Size));
		glVertexAttribPointer(INSTANCED_INV_MODEL_MATRIX_LOCATION + 2, 4, GL_FLOAT, GL_FALSE, instanceDataSize, (void*)(6 * vec4Size));
		glVertexAttribPointer(INSTANCED_INV_MODEL_MATRIX_LOCATION + 3, 4, GL_FLOAT, GL_FALSE, instanceDataSize, (void*)(7 * vec4Size));

		glVertexAttribDivisor(INSTANCED_INV_MODEL_MATRIX_LOCATION + 0, 1);
		glVertexAttribDivisor(INSTANCED_INV_MODEL_MATRIX_LOCATION + 1, 1);
		glVertexAttribDivisor(INSTANCED_INV_MODEL_MATRIX_LOCATION + 2, 1);
		glVertexAttribDivisor(INSTANCED_INV_MODEL_MATRIX_LOCATION + 3, 1);
	}

	void Geometry::InitLightInstancedBuffer()
	{
		glGenBuffers(1, &instancedBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, instancedBuffer);
		std::size_t vec4Size = sizeof(glm::vec4);
		std::size_t instanceDataSize = sizeof(LightInstancedData);

		// ModelMatrix:
		glEnableVertexAttribArray(INSTANCED_MODEL_MATRIX_LOCATION + 0);
		glEnableVertexAttribArray(INSTANCED_MODEL_MATRIX_LOCATION + 1);
		glEnableVertexAttribArray(INSTANCED_MODEL_MATRIX_LOCATION + 2);
		glEnableVertexAttribArray(INSTANCED_MODEL_MATRIX_LOCATION + 3);

		glVertexAttribPointer(INSTANCED_MODEL_MATRIX_LOCATION + 0, 4, GL_FLOAT, GL_FALSE, instanceDataSize, (void*)0);
		glVertexAttribPointer(INSTANCED_MODEL_MATRIX_LOCATION + 1, 4, GL_FLOAT, GL_FALSE, instanceDataSize, (void*)(1 * vec4Size));
		glVertexAttribPointer(INSTANCED_MODEL_MATRIX_LOCATION + 2, 4, GL_FLOAT, GL_FALSE, instanceDataSize, (void*)(2 * vec4Size));
		glVertexAttribPointer(INSTANCED_MODEL_MATRIX_LOCATION + 3, 4, GL_FLOAT, GL_FALSE, instanceDataSize, (void*)(3 * vec4Size));

		glVertexAttribDivisor(INSTANCED_MODEL_MATRIX_LOCATION + 0, 1);
		glVertexAttribDivisor(INSTANCED_MODEL_MATRIX_LOCATION + 1, 1);
		glVertexAttribDivisor(INSTANCED_MODEL_MATRIX_LOCATION + 2, 1);
		glVertexAttribDivisor(INSTANCED_MODEL_MATRIX_LOCATION + 3, 1);

		// LightPosition:
		glEnableVertexAttribArray(INSTANCED_LIGHT_LOCATION);
		glVertexAttribPointer(INSTANCED_LIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, instanceDataSize, (void*)(4 * vec4Size));
		glVertexAttribDivisor(INSTANCED_LIGHT_LOCATION, 1);

		// LightPowerDensity:
		glEnableVertexAttribArray(INSTANCED_LIGHT_POWER_DENSITY_LOCATION);
		glVertexAttribPointer(INSTANCED_LIGHT_POWER_DENSITY_LOCATION, 4, GL_FLOAT, GL_FALSE, instanceDataSize, (void*)(5 * vec4Size));
		glVertexAttribDivisor(INSTANCED_LIGHT_POWER_DENSITY_LOCATION, 1);

		
		// ShadowMapIdx:
		glEnableVertexAttribArray(INSTANCED_LIGHT_SHADOW_MAP_IDX_LOCATION);
		glVertexAttribPointer(INSTANCED_LIGHT_SHADOW_MAP_IDX_LOCATION, 1, GL_FLOAT, GL_FALSE, instanceDataSize, (void*)(6 * vec4Size));
		glVertexAttribDivisor(INSTANCED_LIGHT_SHADOW_MAP_IDX_LOCATION, 1);
		
	}

	void Geometry::Draw()
	{
		vao.Bind();
		if (faceCulling) {
			glEnable(GL_CULL_FACE);
			glFrontFace(faceCullingOrietation);
		}
		else {
			glDisable(GL_CULL_FACE);
		}

		glDrawElements(primitiveType, (GLsizei)indices.size(), GL_UNSIGNED_INT, nullptr);
		vao.Unbind();
	}

	void Geometry::DrawInstanced(const std::vector<InstanceData>& instanceData, int visibleCount)
	{
		vao.Bind();
		if (0 == instancedBuffer) {
			InitInstancedBuffer();
		}
		else {
			glBindBuffer(GL_ARRAY_BUFFER, instancedBuffer);
		}
		glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(InstanceData), &instanceData[0], GL_STATIC_DRAW);

		if (faceCulling) {
			glEnable(GL_CULL_FACE);
			glFrontFace(faceCullingOrietation);
		}
		else {
			glDisable(GL_CULL_FACE);
		}
		glDrawElementsInstanced(primitiveType, (GLsizei)indices.size(), GL_UNSIGNED_INT, nullptr, (GLsizei)visibleCount);
		vao.Unbind();
	}

	void Geometry::DrawInstanced(const std::vector<LightInstancedData>& instanceData)
	{
		vao.Bind();
		if (0 == instancedBuffer) {
			InitLightInstancedBuffer();
		}
		else {
			glBindBuffer(GL_ARRAY_BUFFER, instancedBuffer);
		}
		glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(LightInstancedData), &instanceData[0], GL_STATIC_DRAW);

		if (faceCulling) {
			glEnable(GL_CULL_FACE);
			glFrontFace(faceCullingOrietation);
		}
		else {
			glDisable(GL_CULL_FACE);
		}
		glDrawElementsInstanced(primitiveType, (GLsizei)indices.size(), GL_UNSIGNED_INT, nullptr, (GLsizei)instanceData.size());
	}

	void Geometry::SetFaceCulling(bool cull)
	{
		faceCulling = cull;
	}

	void Geometry::SetFaceCullingOrientation(int orientation)
	{
		faceCullingOrietation = orientation;
	}
}