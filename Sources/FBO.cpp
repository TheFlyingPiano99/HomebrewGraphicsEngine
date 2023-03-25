#include "FBO.h"
#include "GlobalInclude.h"
#include "AssetFolderPathManager.h"

namespace Hogra {
	std::vector<FBO*> FBO::fbos = std::vector<FBO*>();

	void FBO::Init()
	{
		glGenFramebuffers(1, &glID);
	}

	FBO::~FBO()
	{
		auto iter = std::find(fbos.begin(), fbos.end(), this);
		if (iter != fbos.end()) {
			fbos.erase(iter);
		}
		Delete();
	}

	FBO::FBO(const FBO& fbo) : glID(fbo.glID), viewport(fbo.viewport)
	{
		fbos.push_back(this);
	}

	FBO& FBO::operator=(const FBO& fbo)
	{
		if (&fbo == this) {
			return *this;
		}
		this->glID = fbo.glID;
		this->viewport = fbo.viewport;
		return *this;
	}

	void FBO::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, glID);
		glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
	}

	void FBO::Unbind() const
	{
		BindDefault();
	}

	void FBO::Delete()
	{
		glDeleteFramebuffers(1, &glID);
	}

	void FBO::LinkTexture(GLenum attachment, const Texture2D& texture, GLint level)
	{
		if (0 == glID) {
			return;
		}
		glm::ivec2 dim = texture.GetDimensions() / (level + 1);
		viewport = glm::vec4(0, 0, dim.x, dim.y);
		Bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture.glID, level);
	}

	void FBO::LinkTexture(GLenum attachment, const TextureCube& texture)
	{
		if (0 == glID) {
			return;
		}
		const glm::ivec2& dim = texture.GetDimensions();
		viewport = glm::vec4(0, 0, dim.x, dim.y);
		Bind();
		glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture.glID, 0);
	}


	void FBO::LinkRBO(GLenum attachment, RBO& rbo)
	{
		Bind();
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rbo.glID);
	}

	void FBO::DisableDrawBuffer() const
	{
		glDrawBuffer(GL_NONE);
	}

	void FBO::DisableReadBuffer() const
	{
		glReadBuffer(GL_NONE);
	}

	void FBO::SelectDrawBuffers(std::vector<GLenum> bufs)
	{
		glDrawBuffers(bufs.size(), &bufs[0]);
	}


	inline void FBO::BindDefault()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, GlobalVariables::windowWidth, GlobalVariables::windowHeight);
	}

	FBO&& FBO::GetDefault()
	{
		FBO fbo;
		fbo.glID = 0;
		fbo.viewport = glm::vec4(0, 0, GlobalVariables::windowWidth, GlobalVariables::windowHeight);
		return std::move(fbo);
	}

	void FBO::SaveToFileAll()
	{	
		int i = 0;
		for (auto item : fbos) {
			item->saveToPPM(AssetFolderPathManager::getInstance()->getSavesFolderPath().append("screenshot").append(std::to_string(i)).append(".ppm"));
			i++;
		}
	}

	/*
	* Source code from: https://stackoverflow.com/questions/31254444/how-to-save-a-texture-as-an-image-file-using-libraries-related-to-opengl
	* Accessed: 2023-03-25
	*/
	void FBO::saveToPPM(std::string& savePath)
	{
		FILE* output_image;
		int output_width = viewport.z;
		int output_height = viewport.w;

		/// READ THE PIXELS VALUES from FBO AND SAVE TO A .PPM FILE
		unsigned char* pixels = (unsigned char*)malloc(output_width * output_height * 3);

		/// READ THE CONTENT FROM THE FBO
		Bind();
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glReadPixels(viewport.x, viewport.y, viewport.z, viewport.w, GL_RGB, GL_UNSIGNED_BYTE, pixels);

		auto errn = fopen_s(&output_image, savePath.c_str(), "wt");
		fprintf(output_image, "P3\n");
		fprintf(output_image, "# Created with Hogra engine\n");
		fprintf(output_image, "%d %d\n", output_width, output_height);
		fprintf(output_image, "255\n");

		const int pixelSize = 3;
		for (int x = 0; x < output_width; x++)
		{
			for (int y = 0; y < output_height; y++)
			{
				int baseIdx = x * output_height * pixelSize + y * pixelSize;
				fprintf(output_image, 
					"%u %u %u ", 
					(unsigned int)pixels[baseIdx], 
					(unsigned int)pixels[baseIdx + 1],
					(unsigned int)pixels[baseIdx + 2]);
			}
			fprintf(output_image, "\n");
		}
		fclose(output_image);
		free(pixels);
		Unbind();
	}
}