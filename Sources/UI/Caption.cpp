#include "Caption.h"
#include "../GeometryFactory.h"
#include "../DebugUtils.h"


void Hogra::Caption::Init(const std::wstring& _text, Font* _font, float _scale, const glm::vec4& _color)
{
	this->text = _text;
	this->scale = _scale;
	this->font = _font;
	this->color = _color;
	this->shaderProgram = ShaderProgramFactory::GetInstance()->GetForwardCaptionProgram();
	this->texture = font->RenderTextIntoTexture(text);
	this->quad = GeometryFactory::GetInstance()->GetSimpleQuad();
}

void Hogra::Caption::UpdateText(const std::wstring& _text)
{
	text = _text;
	Allocator::Delete(texture);
	texture = font->RenderTextIntoTexture(text);
}

void Hogra::Caption::Draw() const {
	if (!isVisible) {
		return;
	}
	shaderProgram->Activate();
	texture->Bind();
	shaderProgram->SetUniform("sceneObject.modelMatrix", modelMatrix);
	shaderProgram->SetUniform("textColor", color);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	
	quad->Draw();
}

const std::wstring& Hogra::Caption::GetText() {
	return text;
}