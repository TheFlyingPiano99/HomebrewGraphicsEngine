#include "RBO.h"

namespace Hogra {

	void RBO::Init(GLenum internalformat, GLsizei width, GLsizei height)
	{
		glGenRenderbuffers(1, &glID);
		glBindRenderbuffer(GL_RENDERBUFFER, glID);
		glRenderbufferStorage(GL_RENDERBUFFER, internalformat, width, height);
	}

	void RBO::Bind()
	{
		glBindRenderbuffer(GL_RENDERBUFFER, glID);
	}

	void RBO::Delete()
	{
		glDeleteRenderbuffers(1, &glID);
	}
}
