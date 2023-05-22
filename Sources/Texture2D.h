#pragma once
#include "Texture.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "ShaderProgram.h"
#include "MemoryManager.h"
#include <filesystem>

namespace Hogra {

	class Texture2D : public Texture
	{
		friend class Allocator;
	public:

		void Init(const std::filesystem::path& path, GLuint unit, GLenum format, GLenum pixelType, bool gammaCorrectionOnFloat = true);

		void Init(const std::vector<glm::vec4>& _bytes, glm::ivec2 _dimensions, GLuint unit, GLenum _format, GLenum _pixelType);

		void Init(const char* _buffer, glm::ivec2 _dimensions, GLuint _unit, GLenum _internalFormat, GLenum _format, GLenum _pixelType);

		void Init(GLint internalformat, glm::ivec2 dimensions, GLuint unit, GLenum format, GLenum pixelType, bool useMipmaps = false);

		~Texture2D() override {
			this->Delete();
		}

		const glm::ivec2 GetDimensions() const;

		// Inherited via Texture
		void Bind() const override;
		void Unbind() const override;

		const std::vector<glm::vec4>& GetBytes() const;

		const glm::vec4& operator()(glm::ivec2 position) const {
			if (position.x >= dimensions.x || position.x < 0
				|| position.y >= dimensions.y || position.y < 0) {
				return nullVector;
			}
			glm::vec4 v = bytes[position.y * dimensions.x + position.x];
			return v;
		}

		const glm::vec4& operator()(glm::vec2 normalisedPosition) const {
			return operator()(glm::ivec2(
				normalisedPosition.x * (dimensions.x - 1),
				normalisedPosition.y * (dimensions.y - 1)));
		}

		void SetData(const std::vector<glm::vec4>& _data);

		void SetFiltering(GLenum filtering) const override;

		void SetMinFilter(GLenum filter) {
			Bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		}

		void SetMagFilter(GLenum filter) {
			Bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
		}

		void GenerateMipmap();

	private:
		glm::ivec2 dimensions;
		glm::vec4 nullVector;
		std::vector<glm::vec4> bytes;
		GLenum format = GL_RGBA;
		GLenum pixelType = GL_FLOAT;
	};
}