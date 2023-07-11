#include "Container.h"
#include "../ShaderProgramFactory.h"
#include "../GeometryFactory.h"

void Hogra::Container::Init(
	Floating contentFloating, VerticalAlignment vAlignment, HorizontalAlignment hAlignment, 
	const glm::vec4& fillColor, const glm::vec4& borderColor
)
{
	this->contentFloating = contentFloating;
	this->verticalAlignment = vAlignment;
	this->horizontalAlignment = hAlignment;
	this->isFixedSize = false;
	this->shaderProgram = ShaderProgramFactory::GetInstance()->GetFlatContainerProgram();
	this->quad = GeometryFactory::GetInstance()->GetSimpleQuad();
	this->fillColor = fillColor;
	this->borderColor = borderColor;
	this->marginLeftRightTopBottom = { 20, 20, 20, 20 };
	this->borderLeftRightTopBottom = { 3, 3, 3, 3 };
}

void Hogra::Container::Draw() const
{
	if (!isVisible) {
		return;
	}
	shaderProgram->Activate();
	shaderProgram->SetUniform("sceneObject.modelMatrix", modelMatrix);
	if (nullptr == backgroundTexture) {
		shaderProgram->SetUniform("fillColor", fillColor);
		shaderProgram->SetUniform("borderColor", borderColor);
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
