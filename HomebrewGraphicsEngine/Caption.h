#pragma once
#include <string>
#include "Font.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ShaderProgramFactory.h"
#include "FBO.h"
#include "Component.h"
#include "glm/gtx/transform.hpp"
#include "VAO.h"
#include "VBO.h"

namespace Hogra {

	class Caption : public Component
	{
	public:

		void Init(const std::string& text, Font* font, glm::vec2 sPos, float scale, const glm::vec4& color);
		
		~Caption() override = default;

		void Draw();

		const std::string& GetText();

		// Inherited via Component
		void EarlyPhysicsUpdate(float dt) override;

		void LatePhysicsUpdate(float dt) override;

		void Update() override;

		ShaderProgram* GetShaderProgram() const;

		bool IsVisible() const;

		void SetIsVisible(bool b);

		enum CaptionPlacing {
			centeredText,
			leftAligned,
			rightAligned
		};

	private:
		VAO vao;
		VBO vbo;
		std::string text;
		glm::vec2 screenPosition;
		float scale;
		Font* font = nullptr;
		glm::vec4 color;
		ShaderProgram* program = nullptr;
		Texture2D* texture = nullptr;
		bool isVisible = true;
		CaptionPlacing placing = CaptionPlacing::centeredText;

	};

}

