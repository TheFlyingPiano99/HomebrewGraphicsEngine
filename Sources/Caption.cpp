#include "Caption.h"
#include "GeometryFactory.h"
#include "DebugUtils.h"
#include "glm/gtx/transform.hpp"


void Hogra::Caption::Init(const std::wstring& _text, Font* _font, glm::vec2 _screenPos, float _scale, const glm::vec4& _color)
{
	this->text = _text;
	this->scale = _scale;
	this->font = _font;
	this->color = _color;
	this->screenPosition = _screenPos;
	this->program = ShaderProgramFactory::GetInstance()->GetForwardCaptionProgram();
	this->texture = font->RenderTextIntoTexture(text);
	this->quad = GeometryFactory::GetInstance()->GetSimpleQuad();
}

void Hogra::Caption::UpdateText(const std::wstring& _text)
{
	text = _text;
	Allocator::Delete(texture);
	texture = font->RenderTextIntoTexture(text);
}

void Hogra::Caption::Draw() {
	if (!isVisible) {
		return;
	}
	program->Activate();
	texture->Bind();
	glm::vec2 pos;
	if (PlacingStyle::absolute == horizontalPlacing) {
		pos.x = screenPosition.x;
	}
	else {
		pos.x = GlobalVariables::windowWidth * screenPosition.x;
	}
	if (PlacingStyle::absolute == verticalPlacing) {
		pos.y = screenPosition.y;
	}
	else {
		pos.y = GlobalVariables::windowHeight * screenPosition.y;
	}
	pos.y = (float)GlobalVariables::windowHeight - pos.y;	// Top is zero.

	if (alignment == CaptionAlignment::rightAligned) {
		pos.x -= scale * (float)texture->GetDimensions().x * 0.5f;
	}
	else if (alignment == CaptionAlignment::leftAligned) {
		pos.x += scale * (float)texture->GetDimensions().x * 0.5f;
	}
	
	glm::mat4 transform = glm::ortho(0.0f, (float)GlobalVariables::windowWidth, 0.0f, (float)GlobalVariables::windowHeight)
		* glm::translate(glm::vec3(pos, 0.0f)) * glm::scale(scale * glm::vec3(texture->GetDimensions(), 1.0f) * 0.5f);
	program->SetUniform("transform", transform);
	program->SetUniform("textColor", color);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	
	quad->Draw();
}

const std::wstring& Hogra::Caption::GetText() {
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
