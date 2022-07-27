#pragma once
#include <string>
#include "Font.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "FBO.h"
#include "Component.h"
#include "glm/gtx/transform.hpp"
#include "VAO.h"
#include "VBO.h"

namespace Hogra {

	class Caption : public Component
	{
	public:
		Caption(const std::string& text, Font* font, ShaderProgram* program, glm::vec2 sPos, float scale, const glm::vec4& color) 
				: Component(), text(text), scale(scale), font(font), color(color), program(program), screenPosition(sPos) {
			texture = font->RenderTextInTexture(text);
			vao.Bind();
			std::vector<glm::vec4> vertices;
			glm::ivec2 dim = texture->getDimensions();
			vertices.push_back(glm::vec4(dim.x, dim.y, 1.0f, 1.0f));//6
			vertices.push_back(glm::vec4(dim.x, 0.0f, 1.0f, 0.0f));	//5
			vertices.push_back(glm::vec4(0.0f, dim.y, 0.0f, 1.0f)); //4
			vertices.push_back(glm::vec4(dim.x, 0.0f, 1.0f, 0.0f));	//3
			vertices.push_back(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));	//2
			vertices.push_back(glm::vec4(0.0f, dim.y, 0.0f, 1.0f)); //1
			vbo = new VBO(vertices);
			vao.LinkAttrib(*vbo, 0, 4, GL_FLOAT, 4 * sizeof(float), 0);
		}
		
		~Caption() {
			if (nullptr != texture) {
				delete texture;
			}
			vbo->Delete();
			delete vbo;
			vao.Delete();
		}

		void Draw() {
			program->Activate();
			texture->Bind();
			glm::mat4 projection = glm::ortho(0.0f, (float)GlobalVariables::renderResolutionWidth, 0.0f, (float)GlobalVariables::renderResolutionHeight)
					* glm::translate(glm::vec3(screenPosition, 0.0f)) * glm::scale(glm::vec3(scale));
			glUniformMatrix4fv(glGetUniformLocation(program->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			glUniform4f(glGetUniformLocation(program->ID, "textColor"), color.r, color.g, color.b, color.a);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_DEPTH_TEST);
			vao.Bind();
			glDrawArrays(GL_TRIANGLES, 0, 6);
			vao.Unbind();
			texture->Unbind();
		}

		const std::string& getText() {
			return text;
		}

		// Inherited via Component
		void Control(float dt) override;
		void Update(float dt) override;

		ShaderProgram* getShaderProgram() const {
			return program;
		}

	private:
		VAO vao;
		VBO* vbo;
		std::string text;
		glm::vec2 screenPosition;
		float scale;
		Font* font = nullptr;
		glm::vec4 color;
		ShaderProgram* program = nullptr;
		Texture2D* texture = nullptr;

	};

}

