#include "PostProcessStage.h"

namespace Hogra {

	void PostProcessStage::Init(std::string& fragmentShaderPath, int contextWidth, int contextHeight) {
		fbo.Init();
		fbo.Bind();
		program.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("fullscreenQuad.vert"),
			"",
			fragmentShaderPath
		);
		colorTexture.Init(GL_RGBA16F, glm::ivec2(contextWidth, contextHeight), 0, GL_RGBA, GL_FLOAT);
		material = Allocator::New<Material>();
		material->Init(&program);
		material->addTexture(&colorTexture);
		mesh = Allocator::New<Mesh>();
		mesh->Init(material, GeometryFactory::GetInstance()->getFullScreenQuad());
		mesh->setDepthTest(false);
		mesh->setStencilTest(false);
		fbo.LinkTexture(GL_COLOR_ATTACHMENT0, colorTexture, 0);
		fbo.Unbind();
		//RBO stencilRBO(GL_STENCIL_COMPONENTS, contextWidth, contextHeight);
		//fbo.LinkRBO(GL_STENCIL_ATTACHMENT, stencilRBO);
	}

	void PostProcessStage::Bind()
	{
		fbo.Bind();
	}

	void PostProcessStage::Draw(const FBO& nextTargetFbo, const Texture2D& depthTexture)
	{
		nextTargetFbo.Bind();
		mesh->Bind();
		glDisable(GL_BLEND);
		depthTexture.Bind();
		for (auto& var : uniformVariables) {
			var->Bind(*(mesh->getMaterial()->GetShaderProgram()));
		}
		mesh->Draw();
		nextTargetFbo.Unbind();
	}

	FBO& PostProcessStage::GetFBO()
	{
		return fbo;
	}

	void PostProcessStage::OnResize(unsigned int contextWidth, unsigned int contextHeight)
	{
		material->clearTextures();
		colorTexture.Delete();
		
		colorTexture.Init(GL_RGBA16F, glm::ivec2(contextWidth, contextHeight), 0, GL_RGBA, GL_FLOAT);
		material->addTexture(&colorTexture);
		fbo.LinkTexture(GL_COLOR_ATTACHMENT0, colorTexture, 0);
		fbo.Unbind();
	}

	void PostProcessStage::SetActive(bool _active) {
		active = _active;
	}

	const Texture2D& PostProcessStage::GetColorTexture() const {
		return colorTexture;
	}

	void PostProcessStage::AddUniformVariable(AbstractUniformVariable* variable)
	{
		uniformVariables.push_back(variable);
	}

}