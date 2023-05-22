#pragma once
#include "Texture.h"

#include<glad/glad.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include <vector>
#include "MemoryManager.h"
#include "ShaderProgram.h"
#include "Texture2D.h"

namespace Hogra {

	class TextureCube : public Texture
	{
		friend class Allocator;
	
		glm::ivec2 dimensions;
		glm::vec4 nullVector;

	public:
		friend class FBO;

		const char* type;

		void Init(std::vector<std::string>& images, GLuint unit, GLuint pixelType);

		void Init(unsigned int resolution, GLuint unit, GLenum _format, GLenum _pixelType, bool useLinearFiltering = false);

		void InitFromEquirectangular(const Texture2D& equirectangularMap, unsigned int unit, GLuint format, GLuint pixelType);

		void InitFromCubeMap(
			const TextureCube& cubemap,
			ShaderProgram& conversionShader,
			unsigned int resolution,
			unsigned int unit,
			GLuint format,
			GLuint pixelType,
			unsigned int maxMipLevels = 1
		);

		~TextureCube() override {
			this->Delete();
		}

		// Binds a texture
		void Bind();
		// Unbinds a texture
		void Unbind();
		// Deletes a texture
		void Delete();

		const glm::ivec2& GetDimensions() const {
			return dimensions;
		}

		// Inherited via Texture
		virtual void Bind() const override;

		virtual void Unbind() const override;

		void SetFiltering(GLenum filtering) const override;

		Texture2D GetFace(unsigned int face);
	};
}