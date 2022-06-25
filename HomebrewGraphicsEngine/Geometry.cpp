#include "Geometry.h"
#include "GlobalInclude.h"
namespace hograengine {

	Geometry::Geometry(std::vector<Vertex>& vertices, std::vector<GLint>& indices) : vertices(vertices), indices(indices)
	{
		VAO.Bind();
		// Generates Vertex Buffer Object and links it to vertices
		VBO VBO(vertices);
		// Generates Element Buffer Object and links it to indices
		EBO EBO(indices);
		// Links VBO attributes such as coordinates and colors to VAO
		VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);	// pos
		VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));	// normal
		VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));	// tangent
		VAO.LinkAttrib(VBO, 3, 3, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));	// bitangent
		VAO.LinkAttrib(VBO, 4, 2, GL_FLOAT, sizeof(Vertex), (void*)(12 * sizeof(float)));	// uv
		// Unbind all to prevent accidentally modifying them
		VAO.Unbind();
		VBO.Unbind();
		EBO.Unbind();
	}

	void Geometry::Draw()
	{
		VAO.Bind();
		if (faceCulling) {
			glEnable(GL_CULL_FACE);
		}
		else {
			glDisable(GL_CULL_FACE);
		}

		glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, nullptr);
	}

	void Geometry::DrawInstanced(const std::vector<InstanceData>& instanceData)
	{
		unsigned int instancedBuffer;
		glGenBuffers(1, &instancedBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, instancedBuffer);
		glBufferData(GL_ARRAY_BUFFER, instanceData.size() * 2 * sizeof(glm::mat4), &instanceData[0], GL_STATIC_DRAW);

		VAO.Bind();
		if (faceCulling) {
			glEnable(GL_CULL_FACE);
		}
		else {
			glDisable(GL_CULL_FACE);
		}

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

		glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, nullptr, (GLsizei)instanceData.size());
		glDeleteBuffers(1, &instancedBuffer);
	}

	void Geometry::setFaceCulling(bool cull)
	{
		faceCulling = cull;
	}
}