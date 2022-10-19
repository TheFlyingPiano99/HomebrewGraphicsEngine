#include "Caption.h"

void Hogra::Caption::Init(const std::string& text, Font* font, glm::vec2 sPos, float scale, const glm::vec4& color) {
	this->text = text;
	this->scale = scale;
	this->font = font;
	this->color = color;
	this->screenPosition = sPos;
	this->program = ShaderProgramFactory::GetInstance()->GetCaptionProgram();
	this->texture = font->RenderTextInTexture(text);
	vao.Init();
	vao.Bind();
	std::vector<glm::vec4> vertices;
	glm::ivec2 dim = texture->getDimensions();
	vertices.push_back(glm::vec4(dim.x, dim.y, 1.0f, 1.0f));//6
	vertices.push_back(glm::vec4(dim.x, 0.0f, 1.0f, 0.0f));	//5
	vertices.push_back(glm::vec4(0.0f, dim.y, 0.0f, 1.0f)); //4
	vertices.push_back(glm::vec4(dim.x, 0.0f, 1.0f, 0.0f));	//3
	vertices.push_back(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));	//2
	vertices.push_back(glm::vec4(0.0f, dim.y, 0.0f, 1.0f)); //1
	vbo.Init(vertices);
	vao.LinkAttrib(vbo, 0, 4, GL_FLOAT, 4 * sizeof(float), 0);
}

void Hogra::Caption::Draw() {
	if (!isVisible) {
		return;
	}
	program->Activate();
	texture->Bind();
	glm::vec2 pos = screenPosition;
	if (placing == CaptionPlacing::centeredText) {
		pos.x -= scale * (float)texture->getDimensions().x / 2.0f;
	}
	else if (placing == CaptionPlacing::rightAligned) {
		pos.x -= scale * (float)texture->getDimensions().x;
	}
	glm::mat4 projection = glm::ortho(0.0f, (float)GlobalVariables::renderResolutionWidth, 0.0f, (float)GlobalVariables::renderResolutionHeight)
		* glm::translate(glm::vec3(pos, 0.0f)) * glm::scale(glm::vec3(scale));
	glUniformMatrix4fv(glGetUniformLocation(program->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform4f(glGetUniformLocation(program->ID, "textColor"), color.r, color.g, color.b, color.a);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	vao.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	vao.Unbind();
	texture->Unbind();
}

const std::string& Hogra::Caption::GetText() {
	return text;
}

void Hogra::Caption::EarlyPhysicsUpdate(float dt)
{
	;
}

void Hogra::Caption::LatePhysicsUpdate(float dt)
{
	;
}

Hogra::ShaderProgram* Hogra::Caption::GetShaderProgram() const {
	return program;
}

bool Hogra::Caption::IsVisible() const {
	return isVisible;
}

void Hogra::Caption::SetIsVisible(bool b) {
	isVisible = b;
}
