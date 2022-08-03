#include "Bloom.h"
#include "AssetFolderPathManager.h"
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>


Hogra::Bloom::Bloom() {
	treshold = 1.0f;
	mixBloom = 0.4f;
	falloff = 0.9f;
}

void Hogra::Bloom::Init(unsigned int width, unsigned int height) {
	prefilterProgram.Init(
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("simple2D-no-projection.vert"),
		"",
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("bloomPrefilter.frag")
	);
	downSampleProgram.Init(
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("simple2D-no-projection.vert"),
		"",
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("bloomDownSampling.frag")
	);
	upSampleProgram.Init(
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("simple2D-no-projection.vert"),
		"",
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("bloomUpSampling.frag")
	);
	recombineProgram.Init(
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("simple2D-no-projection.vert"),
		"",
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("bloomRecombine.frag")
	);
	fbo.Init();
	vao.Init();
	vao.Bind();
	std::vector<glm::vec4> vertices;
	vertices.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));	//6
	vertices.push_back(glm::vec4(1.0f, -1.0f, 1.0f, 0.0f));	//5
	vertices.push_back(glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f)); //4
	vertices.push_back(glm::vec4(1.0f, -1.0f, 1.0f, 0.0f));	//3
	vertices.push_back(glm::vec4(-1.0f, -1.0f, 0.0f, 0.0f));//2
	vertices.push_back(glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f)); //1
	vbo.Init(vertices);
	vao.LinkAttrib(vbo, 0, 4, GL_FLOAT, 4 * sizeof(float), 0);
	onResize(width, height);
}

void Hogra::Bloom::onResize(unsigned int width, unsigned int height) {
	hdrTexture.Delete();
	depthTexture.Delete();
	for (auto& downScaledTexture : downScaledTextures) {
		downScaledTexture.Delete();
	}
	glm::ivec2 dim = glm::ivec2(width, height);
	hdrTexture.Init(GL_RGBA16F, dim, 1, GL_RGBA, GL_FLOAT);
	depthTexture.Init(GL_DEPTH_COMPONENT, dim, 1, GL_DEPTH_COMPONENT, GL_FLOAT);
	int divider = 2;
	for (int i = 0; i < BLOOM_MIP_LEVELS; i++) {
		downScaledTextures[i].Init(GL_R11F_G11F_B10F, dim / divider, 0, GL_RGB, GL_FLOAT);
		divider *= 2;
	}
	fbo.LinkTexture(GL_COLOR_ATTACHMENT0, hdrTexture, 0);
	fbo.LinkTexture(GL_DEPTH_ATTACHMENT, depthTexture, 0);
	fbo.Unbind();
}

void Hogra::Bloom::Draw(const FBO& outFBO) {
	fbo.Bind();
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	prefilterProgram.Activate();
	hdrTexture.Bind();															// Input of the shader
	fbo.LinkTexture(GL_COLOR_ATTACHMENT0, downScaledTextures[0], 0);			// Output of the shader
	glUniform1f(glGetUniformLocation(prefilterProgram.ID, "treshold"), treshold);

	vao.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);	// Prefilter draw call
	
	downSampleProgram.Activate();
	downScaledTextures[0].Bind();											// Input of the shader
	for (int i = 1; i < BLOOM_MIP_LEVELS; i++)
	{
		downScaledTextures[i - 1].Bind();											// Input of the shader
		fbo.LinkTexture(GL_COLOR_ATTACHMENT0, downScaledTextures[i], 0);	// Output of the shader
		glDrawArrays(GL_TRIANGLES, 0, 6);	// Down sample draw call
	}

	upSampleProgram.Activate();
	glUniform1f(glGetUniformLocation(upSampleProgram.ID, "falloff"), falloff);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	for (int i = BLOOM_MIP_LEVELS - 1; i > 0; i--)
	{
		downScaledTextures[i].Bind();											// Input of the shader
		fbo.LinkTexture(GL_COLOR_ATTACHMENT0, downScaledTextures[i - 1], 0);	// Output of the shader
		glDrawArrays(GL_TRIANGLES, 0, 6);	// Up sample draw call
	}
	
	outFBO.Bind();
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	recombineProgram.Activate();
	glUniform1f(glGetUniformLocation(recombineProgram.ID, "mixBloom"), mixBloom);
	downScaledTextures[0].Bind();
	hdrTexture.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);	// Recombine draw call
	vao.Unbind();
}

void Hogra::Bloom::Bind()
{
	fbo.Bind();
	fbo.LinkTexture(GL_COLOR_ATTACHMENT0, hdrTexture, 0);
}
