#include "Caption.h"
#include "GeometryFactory.h"
#include "DebugUtils.h"
#include "glm/gtx/transform.hpp"

void Hogra::Caption::Init(const std::string& text, Font* font, glm::vec2 sPos, float scale, const glm::vec4& color) {
	this->text = text;
	this->scale = scale;
	this->font = font;
	this->color = color;
	this->screenPosition = sPos;
	this->program = ShaderProgramFactory::GetInstance()->GetForwardCaptionProgram();
	this->texture = font->RenderTextInTexture(text);
	this->quad = GeometryFactory::GetInstance()->GetSimpleQuad();
}

void Hogra::Caption::Draw() {
	if (!isVisible) {
		return;
	}
	program->Activate();
	texture->Bind();
	glm::vec2 pos = screenPosition;
	if (alignment == CaptionAlignment::rightAligned) {
		pos.x -= scale * (float)texture->getDimensions().x * 0.5f;
	}
	else if (alignment == CaptionAlignment::leftAligned) {
		pos.x += scale * (float)texture->getDimensions().x * 0.5f;
	}
	
	glm::mat4 transform = glm::ortho(0.0f, (float)GlobalVariables::windowWidth, 0.0f, (float)GlobalVariables::windowHeight)
		* glm::translate(glm::vec3(pos, 0.0f)) * glm::scale(scale * glm::vec3(texture->getDimensions(), 1.0f) * 0.5f);
	program->SetUniform("transform", transform);
	program->SetUniform("textColor", color);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	
	quad->Draw();
}

const std::string& Hogra::Caption::GetText() {
	return text;
}

void Hogra::Caption::EarlyPhysicsUpdate(float dt)
{
	;
}

void Hogra::Caption::LatePhysicsUpdate(float dt)
{
	;
}

Hogra::ShaderProgram* Hogra::Caption::GetShaderProgram() const {
	return program;
}

bool Hogra::Caption::IsVisible() const {
	return isVisible;
}

void Hogra::Caption::SetIsVisible(bool b) {
	isVisible = b;
}
