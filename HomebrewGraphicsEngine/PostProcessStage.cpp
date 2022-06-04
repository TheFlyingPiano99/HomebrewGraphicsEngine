#include "PostProcessStage.h"

void PostProcessStage::Bind() const
{
	fbo.Bind();
}

void PostProcessStage::Draw(const FBO& nextTargetFbo) const
{
	nextTargetFbo.Bind();
	mesh->Bind();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	mesh->Draw();
}

const FBO& PostProcessStage::getFBO() const
{
	return fbo;
}

void PostProcessStage::resize(int contextWidth, int contextHeight)
{
	material->clearTextures();
	delete colorTexture;
	delete depthTexture;

	colorTexture = new Texture2D(GL_RGBA, glm::ivec2(contextWidth, contextHeight), 0, GL_RGBA, GL_FLOAT);
	depthTexture = new Texture2D(GL_DEPTH_COMPONENT, glm::ivec2(contextWidth, contextHeight), 1, GL_DEPTH_COMPONENT, GL_FLOAT);
	material->addTexture(colorTexture);
	material->addTexture(depthTexture);
	fbo.LinkTexture(GL_COLOR_ATTACHMENT0, *colorTexture, 0);
	fbo.LinkTexture(GL_DEPTH_ATTACHMENT, *depthTexture, 0);
}

void PostProcessStage::setActive(bool _active) {
	active = _active;
}
