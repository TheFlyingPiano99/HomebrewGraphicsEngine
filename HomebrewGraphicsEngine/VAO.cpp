#include"VAO.h"

namespace hograengine {

	// Constructor that generates a VAO ID
	VAO::VAO()
	{
		glGenVertexArrays(1, &ID);
	}

	VAO::~VAO()
	{
		Delete();
	}


	// Links a VBO Attribute such as a position or color to the VAO
	void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
	{
		Bind();
		VBO.Bind();
		glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
		glEnableVertexAttribArray(layout);
		VBO.Unbind();
		Unbind();
	}

	// Binds the VAO
	void VAO::Bind()
	{
		glBindVertexArray(ID);
	}

	// Unbinds the VAO
	void VAO::Unbind()
	{
		glBindVertexArray(0);
	}

	// Deletes the VAO
	void VAO::Delete()
	{
		glDeleteVertexArrays(1, &ID);
	}
}