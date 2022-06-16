#pragma once
#include "Texture.h"

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <vector>

#include "ShaderProgram.h"
namespace hograengine {

	class Texture2D : public Texture
	{
	public:
		const char* type;

		Texture2D(const char* image, const char* type, GLuint unit, GLenum format, GLenum pixelType);

		Texture2D(std::vector<glm::vec4> bytes, glm::ivec2 dimensions, GLuint unit, GLenum format, GLenum pixelType);

		Texture2D(GLint internalformat, glm::ivec2 dimensions, GLuint unit, GLenum format, GLenum pixelType);

		~Texture2D() {
			this->Delete();
		}

		const glm::ivec2 getDimensions();

		const std::vector<glm::vec4>& getBytes();

		glm::vec4& operator()(glm::ivec2 position);

		glm::vec4& operator()(glm::vec2 normalisedPosition);

		// Inherited via Texture
		virtual void Bind() const override;
		virtual void Unbind() const override;

	private:
		std::vector<glm::vec4> bytes;
		glm::ivec2 dimensions;
		glm::vec4 nullVector;
	};
}