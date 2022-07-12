#pragma once
#include <string>
#include "Font.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "FBO.h"

namespace hograengine {

	class Caption
	{
	public:
		Caption(const std::string& text, Font* font, glm::vec2 pos, float scale, const glm::vec4& color) : text(text), font(font) {
			glm::ivec2 dim = font->getTextDimension(text, scale);
			texture = new Texture2D(GL_RGBA, dim, 0, GL_RGBA, GL_FLOAT);
			FBO fbo;
			fbo.LinkTexture(GL_COLOR_ATTACHMENT0, *texture);
			fbo.Bind();
			//TODO: Wrong position coordinates!
			font->RenderText(text, pos.x, pos.y, scale, color);
			fbo.Unbind();
		}
		
		~Caption() {
			if (nullptr != texture) {
				delete texture;
			}
			// Don't delete the shaderProgram!
		}

		void draw() {
			program->Activate();
			texture->Bind();
			//TODO
			texture->Unbind();
		}

		const std::string& getText() {
			return text;
		}

	private:
		std::string text;
		glm::vec2 pos;
		Font* font = nullptr;
		ShaderProgram* program = nullptr;
		Texture2D* texture = nullptr;
	};

}

