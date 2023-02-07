#include "Texture1D.h"
#include "MemoryManager.h"

namespace Hogra {

	void Texture1D::Init(unsigned char* bytes, int width, GLuint slot, GLenum format, GLenum pixelType)
	{
		this->width = width;

		// Generates an OpenGL texture object
		glGenTextures(1, &glID);
		// Assigns the texture to a Texture Unit
		glActiveTexture(GL_TEXTURE0 + slot);
		unit = slot;
		glBindTexture(GL_TEXTURE_1D, glID);

		// Configures the type of algorithm that is used to make the image smaller or bigger
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Configures the way the texture repeats (if it does at all)
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Extra lines in case you choose to use GL_CLAMP_TO_BORDER
		// float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
		// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

		// Assigns the image to the OpenGL Texture object
		glTexImage1D(GL_TEXTURE_1D, 0, format, width, 0, format, pixelType, bytes);
		// Generates MipMaps
		glGenerateMipmap(GL_TEXTURE_1D);

		// Unbinds the OpenGL Texture object so that it can't accidentally be modified
		glBindTexture(GL_TEXTURE_1D, 0);
	}

	void Texture1D::Bind() const
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_1D, glID);
	}

	void Texture1D::Unbind() const
	{
		glBindTexture(GL_TEXTURE_1D, 0);
	}

	void Texture1D::SetFiltering(GLenum filtering) const
	{
		Bind();
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, filtering);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, filtering);
		Unbind();
	}

}