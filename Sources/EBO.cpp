#include"EBO.h"
namespace Hogra {

	void EBO::Init(const std::vector<GLuint>& indices)
	{
		glGenBuffers(1, &glID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	}

	void EBO::Load(const std::vector<GLuint>& indices)
	{
		glDeleteBuffers(1, &glID);
		glGenBuffers(1, &glID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	}


	// Binds the EBO
	void EBO::Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glID);
	}

	// Unbinds the EBO
	void EBO::Unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// Deletes the EBO
	void EBO::Delete()
	{
		glDeleteBuffers(1, &glID);
	}
}