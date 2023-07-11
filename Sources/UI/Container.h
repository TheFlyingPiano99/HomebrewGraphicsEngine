#pragma once
#include "UIElement.h"
#include "../Texture2D.h"

namespace Hogra {

	class Container : public UIElement
	{
	public:
		void Init(
			UIElement::Floating contentFloating, UIElement::VerticalAlignment vAlignment, UIElement::HorizontalAlignment hAlignment,
			const glm::vec4& fillColor = glm::vec4(0.02f, 0.02f, 0.02f, 0.6f), const glm::vec4& borderColor = glm::vec4(0.1f, 0.1f, 0.1f, 0.6f)
		);

		void Draw() const override;

		void SetFillColor(const glm::vec4& color) {
			fillColor = color;
		}

		void SetBorderColor(const glm::vec4& color) {
			borderColor = color;
		}

	private:
		
		glm::vec4 fillColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);
		glm::vec4 borderColor = glm::vec4(0.6f, 0.6f, 0.6f, 0.5f);
		Texture2D* backgroundTexture = nullptr;
	};
}