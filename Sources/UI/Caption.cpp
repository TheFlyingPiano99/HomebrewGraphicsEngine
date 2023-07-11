#include "Caption.h"
#include "../GeometryFactory.h"
#include "../DebugUtils.h"


void Hogra::Caption::Init(const std::wstring& _text, Font* _font, float _scale, const glm::vec4& _textColor, const glm::vec4& _highlightColor)
{
	this->scale = _scale;
	this->font = _font;
	this->textColor = _textColor;
	this->highlightColor = _highlightColor;
	this->shaderProgram = ShaderProgramFactory::GetInstance()->GetForwardCaptionProgram();
	this->quad = GeometryFactory::GetInstance()->GetSimpleQuad();
	this->isFixedSize = true;
	this->marginLeftRightTopBottom = { 20, 20, 10, 10 };
	UpdateText(_text);
}

void Hogra::Caption::UpdateText(const std::wstring& _text)
{
	text = _text;
	Allocator::Delete(texture);
	texture = font->RenderTextIntoTexture(text);
	widthHeight = glm::vec2(texture->GetDimensions()) * scale;
}

void Hogra::Caption::SetTextColor(const glm::vec4& color)
{
	textColor = color;
}

void Hogra::Caption::SetHighlightColor(const glm::vec4& color)
{
	highlightColor = color;
}

void Hogra::Caption::Draw() const {
	if (!isVisible) {
		return;
	}
	shaderProgram->Activate();
	texture->Bind();
	shaderProgram->SetUniform("sceneObject.modelMatrix", modelMatrix);
	shaderProgram->SetUniform("textColor", textColor);
	shaderProgram->SetUniform("highlightColor", highlightColor);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	
	quad->Draw();
}

const std::wstring& Hogra::Caption::GetText() {
	return text;
}