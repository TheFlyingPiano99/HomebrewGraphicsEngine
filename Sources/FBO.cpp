#include "FBO.h"
#include "GlobalInclude.h"
namespace Hogra {

	void FBO::Init()
	{
		glGenFramebuffers(1, &glID);
	}

	FBO::~FBO()
	{
		Delete();
	}

	FBO::FBO(const FBO& fbo)
	{
		this->glID = fbo.glID;
		this->viewport = fbo.viewport;
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
}