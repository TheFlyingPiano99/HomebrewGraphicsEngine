#pragma once
#include<glad/glad.h>
#include "Texture2D.h"
#include "TextureCube.h"
#include "RBO.h"
#include "MemoryManager.h"



namespace Hogra {

	class FBO
	{
		ALLOCATOR_CONSTRUCTIBLE
			
		static std::vector<FBO*> fbos;

	public:
		GLuint glID = 0;
		glm::ivec4 viewport;

		void Init();

		~FBO();

		FBO() {
			fbos.push_back(this);
		};
		
		FBO(const FBO& fbo);

		FBO& operator=(const FBO& fbo);

		void Bind() const;

		void Unbind() const;

		void Delete();

		void LinkTexture(GLenum attachment, const Texture2D& texture, GLint level = 0);

		void LinkTexture(GLenum attachment, const TextureCube& texture);

		void LinkRBO(GLenum attachment, RBO& rbo);

		void DisableDrawBuffer() const;

		void DisableReadBuffer() const;

		/*
		* NO BINDING!!!
		*/
		void SelectDrawBuffers(std::vector<GLenum> bufs);

		inline static void BindDefault();

		static FBO&& GetDefault();

		static void SaveToFileAll();

		void SetViewport(int x0, int y0, int x1, int y1);

		/*
		* Source code from: https://stackoverflow.com/questions/31254444/how-to-save-a-texture-as-an-image-file-using-libraries-related-to-opengl
		* Accessed: 2023-03-25
		*/
		void saveToPPM(const std::string& savePath);

	};

}