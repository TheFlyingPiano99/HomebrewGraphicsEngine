#include "MenuButton.h"
#include "Caption.h"
#include "../GeometryFactory.h"

void Hogra::MenuButton::Init(const std::wstring& text, Font* font)
{
	Caption* caption = Allocator::New<Caption>();
	caption->Init(text, font, 1.0f, {0.9f, 0.9f, 0.9f, 1.0f});
	this->AddChild(caption);

	this->verticalAlignment = UIElement::VerticalAlignment::centered;
	this->horizontalAlignment = UIElement::HorizontalAlignment::centered;
	this->isFixedSize = false;
	this->shaderProgram = ShaderProgramFactory::GetInstance()->GetFlatContainerProgram();
	this->quad = GeometryFactory::GetInstance()->GetSimpleQuad();
	this->marginLeftRightTopBottom = { 20, 20, 20, 20 };
	this->borderLeftRightTopBottom = { 3, 3, 3, 3 };

}


void Hogra::MenuButton::Draw() const
{
	if (!isVisible) {
		return;
	}
	shaderProgram->Activate();
	shaderProgram->SetUniform("sceneObject.modelMatrix", modelMatrix);
	if (nullptr == backgroundTexture) {
		shaderProgram->SetUniform("fillColor", (isHovered) ? hoverFillColor : fillColor);
		shaderProgram->SetUniform("borderColor", (isHovered)? hoverBorderColor : borderColor);
		shaderProgram->SetUniform("widthHeight", widthHeight);
		shaderProgram->SetUniform("borderLeftRightTopBottom", borderLeftRightTopBottom);
	}
	else {
		shaderProgram->SetUniform("backgroundTexture", backgroundTexture);
	}
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	quad->Draw();

	for (auto child : children) {
		child->Draw();
	}
}

void Hogra::MenuButton::ExecuteClickAction(const glm::ivec2& screenCursorPos)
{
	action(screenCursorPos);
}
