#pragma once
#include<glad/glad.h>
#include "Texture2D.h"
#include "RBO.h"
namespace Hogra {

	class FBO
	{
	public:
		GLuint glID = 0;
		glm::ivec4 viewport;

		void Init();

		~FBO();

		FBO() = default;
		
		FBO(const FBO& fbo);

		FBO& operator=(const FBO& fbo);

		void Bind() const;
		void Unbind() const;
		void Delete();

		void LinkTexture(GLenum attachment, const Texture2D& texture, GLint level = 0);

		void LinkRBO(GLenum attachment, RBO& rbo);

		/*
		* NO BINDING!!!
		*/
		void SelectDrawBuffers(std::vector<GLenum> bufs);

		static void BindDefault();
		static FBO&& GetDefault();
	};

}