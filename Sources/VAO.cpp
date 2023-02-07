#include"VAO.h"

namespace Hogra {


	void VAO::Init()
	{
		glGenVertexArrays(1, &glID);
	}

	VAO::~VAO()
	{
		this->Delete();
	}


	// Links a VBO Attribute such as a position or color to the VAO
	void VAO::LinkAttrib(VBO& vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
	{
		Bind();
		vbo.Bind();
		glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
		glEnableVertexAttribArray(layout);
		vbo.Unbind();
		Unbind();
	}

	// Binds the VAO
	void VAO::Bind()
	{
		glBindVertexArray(glID);
	}

	// Unbinds the VAO
	void VAO::Unbind()
	{
		glBindVertexArray(0);
	}

	// Deletes the VAO
	void VAO::Delete()
	{
		glDeleteVertexArrays(1, &glID);
	}
}