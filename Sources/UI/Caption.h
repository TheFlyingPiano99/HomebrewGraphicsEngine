#pragma once
#include "UIElement.h"
#include <string>
#include "Font.h"
#include "../Texture2D.h"
#include "../ShaderProgram.h"
#include "../ShaderProgramFactory.h"
#include "../FBO.h"
#include "../VAO.h"
#include "../VBO.h"
#include "../Geometry.h"

namespace Hogra {

	class Caption : public UIElement
	{
	public:

		void Init(const std::wstring& text, Font* font, float scale, const glm::vec4& color);
		
		void Draw() const override;

		void UpdateText(const std::wstring& str);

		const std::wstring& GetText();

	private:
		std::wstring text;
		Font* font = nullptr;
		glm::vec4 color;
		Texture2D* texture = nullptr;
		float scale = 1.0f;
	};
}

