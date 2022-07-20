#include "Bloom.h"
#include "AssetFolderPathManager.h"
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>

#define BLOOM_RESOLUTION_WIDTH 512
#define BLOOM_RESOLUTION_HEIGHT 512

hograengine::Bloom::Bloom() {
	treshold = 1.0f;
	intensity = 1.0f;
}

hograengine::Bloom::~Bloom() {
	if (nullptr != ubo) {
		delete ubo;
		delete program;
		delete vbo;
		glDeleteTextures(1, & brightTexture);
	}
}

void hograengine::Bloom::init(unsigned int width, unsigned int height) {
	std::vector<int> subdataSizes;
	subdataSizes.push_back(sizeof(float));		// brightness treshold
	ubo = new UniformBufferObject(subdataSizes, 3);
	program = new ShaderProgram(
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("simple2D.vert"),
		"",
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("gausianBlur.frag")
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

	// Init Pingpong:
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongBuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glViewport(0, 0, BLOOM_RESOLUTION_WIDTH, BLOOM_RESOLUTION_HEIGHT);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffers[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA16F, BLOOM_RESOLUTION_WIDTH, BLOOM_RESOLUTION_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffers[i], 0
		);
	}

	onResize(width, height);
}

void hograengine::Bloom::BindBrightTexture() {
	ubo->Bind();
	ubo->uploadSubData(&treshold, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, brightTexture, 0);
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
}

void hograengine::Bloom::UnbindBrightTexture() {
	ubo->Unbind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0);
	unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);
}

void hograengine::Bloom::draw(const FBO& fbo) {
	program->Activate();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, brightTexture);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glm::mat4 projection(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(program->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform1f(glGetUniformLocation(program->ID, "intensity"), intensity);
	vao.Bind();
	bool horizontal = true, first_iteration = true;
	int amount = 10;
	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		glViewport(0, 0, BLOOM_RESOLUTION_WIDTH, BLOOM_RESOLUTION_HEIGHT);
		glUniform1i(glGetUniformLocation(program->ID, "horizontal"), horizontal);
		glBindTexture(
			GL_TEXTURE_2D, (first_iteration)? brightTexture : pingpongBuffers[!horizontal]
		);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	fbo.Bind();
	glUniform1i(glGetUniformLocation(program->ID, "horizontal"), horizontal);
	glBindTexture(
		GL_TEXTURE_2D, pingpongBuffers[!horizontal]
	);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_BLEND);
	vao.Unbind();
}

void hograengine::Bloom::onResize(unsigned int width, unsigned int height) {
	if (0 != brightTexture) {
		glDeleteTextures(1, &brightTexture);
	}
	glGenTextures(1, &brightTexture);
	glBindTexture(GL_TEXTURE_2D, brightTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

}
