#pragma once
#include "Texture.h"

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <vector>
#include "ShaderProgram.h"
#include "MemoryManager.h"

namespace Hogra {

	class Texture2D : public Texture
	{
	public:

		static Texture2D* Instantiate();

		void Init(const std::string& path, GLuint unit, GLenum format, GLenum pixelType);

		void Init(const std::vector<glm::vec4>& bytes, glm::ivec2 dimensions, GLuint unit, GLenum format, GLenum pixelType);

		void Init(GLint internalformat, glm::ivec2 dimensions, GLuint unit, GLenum format, GLenum pixelType);

		~Texture2D() override {
			this->Delete();
		}

		const glm::ivec2 getDimensions() const;

		// Inherited via Texture
		void Bind() const override;
		void Unbind() const override;

		const std::vector<glm::vec4>& GetBytes() const;

		glm::vec4& operator()(glm::ivec2 position) {
			if (position.x >= dimensions.x || position.x < 0
				|| position.y >= dimensions.y || position.y < 0) {
				return nullVector;
			}
			glm::vec4 v = bytes[position.y * dimensions.x + position.x];
			return v;
		}

		glm::vec4& operator()(glm::vec2 normalisedPosition) {
			return operator()(glm::ivec2(
				normalisedPosition.x * (dimensions.x - 1),
				normalisedPosition.y * (dimensions.y - 1)));
		}

	private:
		glm::ivec2 dimensions;
		glm::vec4 nullVector;
		std::vector<glm::vec4> bytes;

		friend class Allocator<Texture2D>;
	};
}