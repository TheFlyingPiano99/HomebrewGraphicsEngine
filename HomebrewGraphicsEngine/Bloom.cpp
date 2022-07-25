#include "Bloom.h"
#include "AssetFolderPathManager.h"
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>

#define BLOOM_RESOLUTION_WIDTH 512
#define BLOOM_RESOLUTION_HEIGHT 512

hograengine::Bloom::Bloom() {
	treshold = 1.0f;
	mixBloom = 0.4f;
	falloff = 0.9f;
}

hograengine::Bloom::~Bloom() {
}

void hograengine::Bloom::init(unsigned int width, unsigned int height) {
	prefilterProgram = new ShaderProgram(
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("simple2D.vert"),
		"",
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("bloomPrefilter.frag")
	);
	downSampleProgram = new ShaderProgram(
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("simple2D.vert"),
		"",
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("bloomDownSampling.frag")
	);
	upSampleProgram = new ShaderProgram(
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("simple2D.vert"),
		"",
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("bloomUpSampling.frag")
	);
	recombineProgram = new ShaderProgram(
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("simple2D.vert"),
		"",
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("bloomRecombine.frag")
	);
	vao.Bind();
	std::vector<glm::vec4> vertices;
	vertices.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));	//6
	vertices.push_back(glm::vec4(1.0f, -1.0f, 1.0f, 0.0f));	//5
	vertices.push_back(glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f)); //4
	vertices.push_back(glm::vec4(1.0f, -1.0f, 1.0f, 0.0f));	//3
	vertices.push_back(glm::vec4(-1.0f, -1.0f, 0.0f, 0.0f));//2
	vertices.push_back(glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f)); //1
	vbo = new VBO(vertices);
	vao.LinkAttrib(*vbo, 0, 4, GL_FLOAT, 4 * sizeof(float), 0);
	onResize(width, height);
}

void hograengine::Bloom::draw(const FBO& outFBO) {
	fbo.Bind();
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	prefilterProgram->Activate();
	hdrTexture->Bind();														// Input of the shader
	fbo.LinkTexture(GL_COLOR_ATTACHMENT0, *downScaledTextures[0], 0);			// Output of the shader
	glm::mat4 projection(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(prefilterProgram->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform1f(glGetUniformLocation(prefilterProgram->ID, "treshold"), treshold);

	vao.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);	// Prefilter draw call
	
	downSampleProgram->Activate();
	downScaledTextures[0]->Bind();											// Input of the shader
	glUniformMatrix4fv(glGetUniformLocation(downSampleProgram->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	int amount = 9;
	for (int i = 1; i <= amount; i++)
	{
		downScaledTextures[i - 1]->Bind();											// Input of the shader
		glUniform1i(glGetUniformLocation(downSampleProgram->ID, "mipLevel"), 0);
		fbo.LinkTexture(GL_COLOR_ATTACHMENT0, *downScaledTextures[i], 0);	// Output of the shader
		glDrawArrays(GL_TRIANGLES, 0, 6);	// Down sample draw call
	}

	upSampleProgram->Activate();
	glUniform1f(glGetUniformLocation(upSampleProgram->ID, "falloff"), falloff);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glUniformMatrix4fv(glGetUniformLocation(upSampleProgram->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	for (int i = amount - 1; i >= 0; i--)
	{
		downScaledTextures[i + 1]->Bind();											// Input of the shader
		glUniform1i(glGetUniformLocation(upSampleProgram->ID, "mipLevel"), 0);
		fbo.LinkTexture(GL_COLOR_ATTACHMENT0, *downScaledTextures[i], 0);	// Output of the shader
		glDrawArrays(GL_TRIANGLES, 0, 6);	// Up sample draw call
	}
	
	outFBO.Bind();
	glDisable(GL_BLEND);
	recombineProgram->Activate();
	glUniformMatrix4fv(glGetUniformLocation(recombineProgram->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform1f(glGetUniformLocation(recombineProgram->ID, "mixBloom"), mixBloom);
	downScaledTextures[0]->Bind();
	hdrTexture->Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);	// Recombine draw call
	vao.Unbind();
}

void hograengine::Bloom::onResize(unsigned int width, unsigned int height) {
	if (nullptr != hdrTexture) {
		delete hdrTexture;
		delete depthTexture;
		for (auto* downScaledTexture : downScaledTextures) {
			delete downScaledTexture;
		}
		downScaledTextures.clear();
	}
	glm::ivec2 dim = glm::ivec2(width, height);
	hdrTexture = new Texture2D(GL_RGBA16F, dim, 1, GL_RGBA, GL_FLOAT);
	depthTexture = new Texture2D(GL_DEPTH_COMPONENT, dim, 1, GL_DEPTH_COMPONENT, GL_FLOAT);
	int divider = 2;
	for (int i = 0; i < 10; i++) {
		downScaledTextures.push_back(new Texture2D(GL_R11F_G11F_B10F, dim / divider, 0, GL_RGB, GL_FLOAT));
		divider *= 2;
	}
	fbo.LinkTexture(GL_COLOR_ATTACHMENT0, *hdrTexture, 0);
	fbo.LinkTexture(GL_DEPTH_ATTACHMENT, *depthTexture, 0);
	fbo.Unbind();
}

void hograengine::Bloom::Bind()
{
	fbo.Bind();
	fbo.LinkTexture(GL_COLOR_ATTACHMENT0, *hdrTexture, 0);
}
