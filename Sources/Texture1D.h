#pragma once
#include "Texture.h"
#include<glad/glad.h>
#include<stb/stb_image.h>

#include"ShaderProgram.h"
#include "MemoryManager.h"

namespace Hogra {

	class Texture1D : public Texture
	{
		friend class Allocator;
	public:
		const char* type;
		unsigned int width = 0;

		void Init(unsigned char* bytes, int width, GLuint slot, GLenum format, GLenum pixelType);

		~Texture1D() override {
			this->Delete();
		}

		const unsigned int getWidth() {
			return width;
		}

		// Inherited via Texture
		void Bind() const override;
		void Unbind() const override;
		void SetFiltering(GLenum filtering) const override;
	};
}