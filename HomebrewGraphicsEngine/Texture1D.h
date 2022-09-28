#pragma once
#include "Texture.h"
#include<glad/glad.h>
#include<stb/stb_image.h>

#include"ShaderProgram.h"
#include "MemoryManager.h"

namespace Hogra {

	class Texture1D : public Texture
	{
		friend class Allocator<Texture1D>;
	public:
		const char* type;
		unsigned int width = 0;

		void Init(unsigned char* bytes, int width, GLuint slot, GLenum format, GLenum pixelType);

		~Texture1D() override {
		}

		const unsigned int getWidth() {
			return width;
		}

		// Inherited via Texture
		virtual void Bind() const override;
		virtual void Unbind() const override;
	};
}