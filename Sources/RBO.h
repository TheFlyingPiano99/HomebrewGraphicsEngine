#pragma once
#include <glad/glad.h>
#include <vector>
namespace Hogra {

	class RBO
	{
	public:
		friend class FBO;
		GLuint glID;

		void Init(GLenum internalformat, GLsizei width, GLsizei height);

		void Bind();

		void Delete();
	};

}