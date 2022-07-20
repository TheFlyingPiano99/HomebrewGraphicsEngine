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

		Texture2D(const std::string& path, GLuint unit, GLenum format, GLenum pixelType);

		Texture2D(std::vector<glm::vec4> bytes, glm::ivec2 dimensions, GLuint unit, GLenum format, GLenum pixelType);

		Texture2D(GLint internalformat, glm::ivec2 dimensions, GLuint unit, GLenum format, GLenum pixelType);

		~Texture2D() {
			this->Delete();
		}

		const glm::ivec2 getDimensions() const;

		const std::vector<glm::vec4>& getBytes() const;

		glm::vec4& operator()(glm::ivec2 position);

		glm::vec4& operator()(glm::vec2 normalisedPosition);

		// Inherited via Texture
		void Bind() const override;
		void Unbind() const override;

	private:
		std::vector<glm::vec4> bytes;
		glm::ivec2 dimensions;
		glm::vec4 nullVector;
	};
}