#pragma once

#include<glad/glad.h>
#include"VBO.h"
namespace Hogra {

	class VAO
	{
	public:
		// ID reference for the Vertex Array Object
		GLuint glID;

		void Init();

		~VAO();

		// Links a VBO Attribute such as a position or color to the VAO
		void LinkAttrib(VBO& vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
		// Binds the VAO
		void Bind();
		// Unbinds the VAO
		void Unbind();
		// Deletes the VAO
		void Delete();
	};
}
