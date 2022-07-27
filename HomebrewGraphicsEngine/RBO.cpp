#include "RBO.h"

namespace Hogra {

	RBO::RBO(GLenum internalformat, GLsizei width, GLsizei height)
	{
		glGenRenderbuffers(1, &ID);
		glBindRenderbuffer(GL_RENDERBUFFER, ID);
		glRenderbufferStorage(GL_RENDERBUFFER, internalformat, width, height);
	}

	void RBO::Bind()
	{
		glBindRenderbuffer(GL_RENDERBUFFER, ID);
	}

	void RBO::Delete()
	{
		glDeleteRenderbuffers(1, &ID);
	}
}
