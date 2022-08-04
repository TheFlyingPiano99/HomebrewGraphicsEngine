#pragma once
#include "Texture.h"

#include<glad/glad.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include <vector>

#include"ShaderProgram.h"
namespace Hogra {

	class TextureCube : public Texture
	{
		glm::ivec2 dimensions;
		glm::vec4 nullVector;

	public:
		friend class FBO;

		const char* type;

		static TextureCube* Instantiate();

		void Init(std::vector<std::string>& images, GLuint unit);

		~TextureCube() override {
			this->Delete();
		}

		// Binds a texture
		void Bind();
		// Unbinds a texture
		void Unbind();
		// Deletes a texture
		void Delete();

		const glm::ivec2 getDimensions() {
			return dimensions;
		}

		// Inherited via Texture
		virtual void Bind() const override;

		virtual void Unbind() const override;
	};
}