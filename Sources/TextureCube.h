#pragma once
#include "Texture.h"

#include<glad/glad.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include <vector>
#include "MemoryManager.h"
#include"ShaderProgram.h"

namespace Hogra {

	class TextureCube : public Texture
	{
		friend class Allocator;
	
		glm::ivec2 dimensions;
		glm::vec4 nullVector;

	public:
		friend class FBO;

		const char* type;

		void Init(std::vector<std::string>& images, GLuint unit);

		void Init(unsigned int resolution, GLuint unit, GLenum _format, GLenum _pixelType);

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
	};
}