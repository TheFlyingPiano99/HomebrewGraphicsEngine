#include "PostProcessStage.h"

namespace Hogra {

	PostProcessStage::PostProcessStage(std::string& fragmentShaderPath, int contextWidth, int contextHeight) {
		fbo.Init();
		fbo.Bind();
		program.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("fullscreenQuad.vert"),
			"",
			fragmentShaderPath
		);
		program.Activate();
		colorTexture.Init(GL_RGBA16F, glm::ivec2(contextWidth, contextHeight), 0, GL_RGBA, GL_FLOAT);
		depthTexture.Init(GL_DEPTH_COMPONENT, glm::ivec2(contextWidth, contextHeight), 1, GL_DEPTH_COMPONENT, GL_FLOAT);
		material = Material::Instantiate();
		material->Init(&program);
		material->addTexture(&colorTexture);
		material->addTexture(&depthTexture);
		mesh = Mesh::Instantiate();
		mesh->Init(material, GeometryFactory::GetInstance()->getFullScreenQuad());
		mesh->setDepthTest(false);
		mesh->setStencilTest(false);
		fbo.LinkTexture(GL_COLOR_ATTACHMENT0, colorTexture, 0);
		fbo.LinkTexture(GL_DEPTH_ATTACHMENT, depthTexture, 0);
		fbo.Unbind();
		//RBO stencilRBO(GL_STENCIL_COMPONENTS, contextWidth, contextHeight);
		//fbo.LinkRBO(GL_STENCIL_ATTACHMENT, stencilRBO);
	}

	void PostProcessStage::Bind() const
	{
		fbo.Bind();
	}

	void PostProcessStage::Draw(const FBO& nextTargetFbo) const
	{
		nextTargetFbo.Bind();
		mesh->Bind();
		glDisable(GL_BLEND);
		mesh->Draw();
	}

	const FBO& PostProcessStage::getFBO() const
	{
		return fbo;
	}

	void PostProcessStage::onResize(int contextWidth, int contextHeight)
	{
		material->clearTextures();
		colorTexture.Delete();
		depthTexture.Delete();
		
		colorTexture.Init(GL_RGBA16F, glm::ivec2(contextWidth, contextHeight), 0, GL_RGBA, GL_FLOAT);
		depthTexture.Init(GL_DEPTH_COMPONENT, glm::ivec2(contextWidth, contextHeight), 1, GL_DEPTH_COMPONENT, GL_FLOAT);
		material->addTexture(&colorTexture);
		material->addTexture(&depthTexture);
		fbo.LinkTexture(GL_COLOR_ATTACHMENT0, colorTexture, 0);
		fbo.LinkTexture(GL_DEPTH_ATTACHMENT, depthTexture, 0);
		fbo.Unbind();
	}

	void PostProcessStage::setActive(bool _active) {
		active = _active;
	}

}