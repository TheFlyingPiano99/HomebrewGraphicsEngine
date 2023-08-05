/*
* ~~~~~~~~~~~~~~~~~~~~~~~~~
* Homebrew Graphics Engine
* Zoltan Simon 2023
* Repository: https://github.com/TheFlyingPiano99/HomebrewGraphicsEngine
* ~~~~~~~~~~~~~~~~~~~~~~~~~
*/
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
		ALLOCATOR_CONSTRUCTIBLE

	public:

		void Init(const std::filesystem::path& path, GLuint unit, GLenum clientDataFormat, GLenum clientDataType, bool gammaCorrectionOnFloat = true, bool useMipMaps = false);

		void Init(const std::vector<glm::vec4>& _bytes, glm::ivec2 _dimensions, GLuint unit, GLenum _format, GLenum _pixelType, bool useMipmaps = false);

		void Init(const char* _buffer, glm::ivec2 _dimensions, GLuint _unit, GLenum _internalFormat, GLenum _format, GLenum _pixelType);

		void Init(glm::ivec2 dimensions, GLuint unit, GLint internalformat, GLenum clientDataFormat, GLenum clientDataType, bool useMipmaps = false);

		~Texture2D() override {
			if (auto iter = std::find(allTexture2Ds.begin(), allTexture2Ds.end(), this); iter != allTexture2Ds.end()) {
				allTexture2Ds.erase(iter);
			}
			this->Delete();
		}

		const glm::ivec2 GetDimensions() const;

		// Inherited via Texture
		void Bind() const override;

		void Unbind() const override;

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

		void SetFiltering(GLenum filtering) const override;

		void SetMinFilter(GLenum filter) {
			Bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		}

		void SetMagFilter(GLenum filter) {
			Bind();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
		}

		void GenerateMipmap() const;

		void WriteData(void* dataPtr) override;

		void ReadData(void* dataPtr) const override;

		void SaveToPPM(const std::filesystem::path& path);

		static void SaveAllToPPM();

	private:
		glm::ivec2 dimensions;
		glm::vec4 nullVector;
		std::vector<glm::vec4> bytes;
		static std::vector<Texture2D*> allTexture2Ds;
	};
}