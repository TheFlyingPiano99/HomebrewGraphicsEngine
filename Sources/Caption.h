#pragma once
#include <string>
#include "Font.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ShaderProgramFactory.h"
#include "FBO.h"
#include "Component.h"
#include "VAO.h"
#include "VBO.h"
#include "Geometry.h"

namespace Hogra {

	class Caption : public Component
	{
	public:

		void Init(const std::wstring& text, Font* font, glm::vec2 sPos, float scale, const glm::vec4& color);
		
		~Caption() override = default;

		void UpdateText(const std::wstring& str);

		void Draw();

		const std::wstring& GetText();

		// Inherited via Component
		void EarlyPhysicsUpdate(float dt) override;

		void LatePhysicsUpdate(float dt) override;

		ShaderProgram* GetShaderProgram() const;

		bool IsVisible() const;

		void SetIsVisible(bool b);

		enum class CaptionAlignment {
			centeredText,
			leftAligned,
			rightAligned
		};

		enum class PlacingStyle {
			relative,
			absolute,
		};

		void SetHorizontalPlacingStyle(PlacingStyle style) {
			horizontalPlacing = style;
		}
		void SetVerticalPlacingStyle(PlacingStyle style) {
			verticalPlacing = style;
		}

	private:
		Geometry* quad;
		std::wstring text;
		PlacingStyle horizontalPlacing = PlacingStyle::absolute;
		PlacingStyle verticalPlacing = PlacingStyle::absolute;
		glm::vec2 screenPosition;
		float scale;
		Font* font = nullptr;
		glm::vec4 color;
		ShaderProgram* program = nullptr;
		Texture2D* texture = nullptr;
		bool isVisible = true;
		CaptionAlignment alignment = CaptionAlignment::centeredText;

	};

}

