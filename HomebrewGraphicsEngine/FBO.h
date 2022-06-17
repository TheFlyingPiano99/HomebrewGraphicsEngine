#pragma once
#include<glad/glad.h>
#include "Texture2D.h"
#include "RBO.h"
namespace hograengine {

	class FBO
	{
	public:
		GLuint ID;
		glm::ivec4 viewport;

		FBO();

		void Bind() const;
		void Unbind() const;
		void Delete();

		void LinkTexture(GLenum attachment, Texture2D& texture, GLint level = 0);

		void LinkRBO(GLenum attachment, RBO& rbo);

		/*
		* NO BINDING!!!
		*/
		void SelectDrawBuffers(std::vector<GLenum> bufs);

		static void BindDefault();
		static FBO getDefault();
	};

}