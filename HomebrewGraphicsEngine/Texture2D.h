#pragma once
#include "Texture.h"

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <vector>
#include "ShaderProgram.h"

namespace Hogra {

	class Texture2D : public Texture
	{
	public:

		static Texture2D* Instantiate();

		void Init(const std::string& path, GLuint unit, GLenum format, GLenum pixelType);

		void Init(std::vector<glm::vec4> bytes, glm::ivec2 dimensions, GLuint unit, GLenum format, GLenum pixelType);

		void Init(GLint internalformat, glm::ivec2 dimensions, GLuint unit, GLenum format, GLenum pixelType);

		~Texture2D() override {
			this->Delete();
		}

		const glm::ivec2 getDimensions() const;

		// Inherited via Texture
		void Bind() const override;
		void Unbind() const override;

	private:
		glm::ivec2 dimensions;
		glm::vec4 nullVector;
	};
}