#pragma once
#include "Texture.h"
#include<glad/glad.h>
#include<stb/stb_image.h>

#include"ShaderProgram.h"

class Texture1D : public Texture
{
public:
	const char* type;
	float shininess = 16.0f;	// Should be extracted to Material
	unsigned char* bytes = nullptr;
	unsigned int width = 0;

	Texture1D(unsigned char* bytes, int width, GLuint slot, GLenum format, GLenum pixelType);
	~Texture1D() {
		if (bytes != nullptr) {
			delete[] bytes;
		}
	}

	const unsigned char* getBytes() {
		return bytes;
	}

	const unsigned int getWidth() {
		return width;
	}

	// Inherited via Texture
	virtual void Bind() const override;
	virtual void Unbind() const override;
};
