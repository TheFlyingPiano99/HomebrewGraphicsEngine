#include "Bloom.h"
#include "AssetFolderPathManager.h"
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include "VBO.h"

Hogra::Bloom::Bloom() {
	treshold = 1.0f;
	mixBloom = 0.4f;
	falloff = 0.9f;
}

void Hogra::Bloom::Init(unsigned int _contextWidth, unsigned int _contextHeight) {
	prefilterProgram.Init(
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("fullScreenQuad.vert"),
		"",
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("bloomPrefilter.frag")
	);
	downSampleProgram.Init(
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("fullScreenQuad.vert"),
		"",
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("bloomDownSampling.frag")
	);
	upSampleProgram.Init(
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("fullScreenQuad.vert"),
		"",
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("bloomUpSampling.frag")
	);
	recombineProgram.Init(
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("fullScreenQuad.vert"),
		"",
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("bloomRecombine.frag")
	);
	fbo.Init();
	quad = GeometryFactory::GetInstance()->GetSimpleQuad();
	OnContextResize(_contextWidth, _contextHeight);
}

void Hogra::Bloom::OnContextResize(unsigned int contextW, unsigned int contextH) {
	hdrTexture.Delete();
	for (auto& downScaledTexture : downScaledTextures) {
		downScaledTexture.Delete();
	}
	glm::ivec2 dim = glm::ivec2(contextW, contextH);
	hdrTexture.Init(GL_RGBA16F, dim, 1, GL_RGBA, GL_FLOAT);
	int divider = 2;
	for (int i = 0; i < BLOOM_MIP_LEVELS; i++) {
		downScaledTextures[i].Init(GL_R11F_G11F_B10F, dim / divider, 0, GL_RGB, GL_FLOAT);
		divider *= 2;
	}
	fbo.LinkTexture(GL_COLOR_ATTACHMENT0, hdrTexture, 0);
	fbo.Unbind();
}

void Hogra::Bloom::Draw(const FBO& outFBO, const Texture2D& depthTexture, const Camera& camera) {
	fbo.Bind();
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	prefilterProgram.Activate();
	hdrTexture.Bind();															// Input of the shader
	fbo.LinkTexture(GL_COLOR_ATTACHMENT0, downScaledTextures[0], 0);			// Output of the shader
	prefilterProgram.SetUniform("treshold", treshold);

	quad->Draw();	// Prefilter draw call
	
	downSampleProgram.Activate();
	downScaledTextures[0].Bind();											// Input of the shader
	for (int i = 1; i < BLOOM_MIP_LEVELS; i++)
	{
		downScaledTextures[i - 1].Bind();											// Input of the shader
		fbo.LinkTexture(GL_COLOR_ATTACHMENT0, downScaledTextures[i], 0);	// Output of the shader
		quad->Draw();	// Down sample draw call
	}

	upSampleProgram.Activate();
	upSampleProgram.SetUniform("falloff", falloff);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	for (int i = BLOOM_MIP_LEVELS - 1; i > 0; i--)
	{
		downScaledTextures[i].Bind();											// Input of the shader
		fbo.LinkTexture(GL_COLOR_ATTACHMENT0, downScaledTextures[i - 1], 0);	// Output of the shader
		quad->Draw();	// Up sample draw call
	}
	// Rebind hdr texture:
	fbo.LinkTexture(GL_COLOR_ATTACHMENT0, hdrTexture, 0);
	outFBO.Bind();
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	recombineProgram.Activate();
	recombineProgram.SetUniform("mixBloom", mixBloom);
	downScaledTextures[0].Bind();
	hdrTexture.Bind();
	quad->Draw();	// Recombine draw call
}

void Hogra::Bloom::Bind()
{
	fbo.Bind();
	fbo.LinkTexture(GL_COLOR_ATTACHMENT0, hdrTexture, 0);
}
