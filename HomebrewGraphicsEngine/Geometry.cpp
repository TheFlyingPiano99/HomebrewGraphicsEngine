#include "Geometry.h"
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

	void Geometry::setFaceCulling(bool cull)
	{
		faceCulling = cull;
	}
}