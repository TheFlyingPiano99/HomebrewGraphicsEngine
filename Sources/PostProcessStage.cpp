#include "PostProcessStage.h"

namespace Hogra {

	void PostProcessStage::Init(std::string& fragmentShaderPath, int contextWidth, int contextHeight) {
		fbo.Init();
		fbo.Bind();
		program.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("fullScreenQuadWithRayDir.vert"),
			"",
			fragmentShaderPath
		);
		colorTexture.Init(GL_RGBA16F, glm::ivec2(contextWidth, contextHeight), 0, GL_RGBA, GL_FLOAT);
		volumeMaterial = Allocator::New<Material>();
		volumeMaterial->Init(&program);
		volumeMaterial->AddTexture(&colorTexture);
		mesh = Allocator::New<Mesh>();
		mesh->Init(volumeMaterial, GeometryFactory::GetInstance()->GetSimpleQuad());
		mesh->SetDepthTest(false);
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

	void PostProcessStage::Draw(const FBO& outFBO, const Texture2D& depthTexture, const Camera& camera)
	{
		outFBO.Bind();
		mesh->Bind();
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		depthTexture.Bind();
		mesh->Draw();
		outFBO.Unbind();
	}

	FBO& PostProcessStage::GetFBO()
	{
		return fbo;
	}

	void PostProcessStage::OnContextResize(unsigned int contextWidth, unsigned int contextHeight)
	{
		volumeMaterial->ClearTextures();
		colorTexture.Delete();
		
		colorTexture.Init(GL_RGBA16F, glm::ivec2(contextWidth, contextHeight), 0, GL_RGBA, GL_FLOAT);
		volumeMaterial->AddTexture(&colorTexture);
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
		if (nullptr != mesh) {
			mesh->getMaterial()->GetShaderProgram()->BindUniformVariable(variable);
		}
	}

}