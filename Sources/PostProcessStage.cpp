#include "PostProcessStage.h"

namespace Hogra {

	void PostProcessStage::Init(const std::filesystem::path& fragmentShaderPath, int contextWidth, int contextHeight) {
		program.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("DefaultPipeline/fullScreenQuadWithRayDir.vert"),
			"",
			fragmentShaderPath
		);
		colorTexture.Init(glm::ivec2(contextWidth, contextHeight), 0, GL_RGBA16F, GL_RGBA, GL_FLOAT);
		volumeMaterial = Allocator::New<Material>();
		volumeMaterial->Init(&program);
		volumeMaterial->AddTexture(&colorTexture);
		volumeMaterial->SetAlphaBlend(false);
		mesh = Allocator::New<Mesh>();
		mesh->Init(volumeMaterial, GeometryFactory::GetInstance()->GetSimpleQuad());
		mesh->SetDepthTest(false);
		mesh->setStencilTest(false);
		fbo.Init();
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
		
		colorTexture.Init(glm::ivec2(contextWidth, contextHeight), 0, GL_RGBA16F, GL_RGBA, GL_FLOAT);
		volumeMaterial->AddTexture(&colorTexture);
		fbo.Delete();
		fbo.Init();
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