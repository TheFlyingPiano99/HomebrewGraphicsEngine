#include "Texture.h"
#include "glad/glad.h"
#include "DebugUtils.h"

namespace Hogra {
	void Texture::BindToImageUnit() const
	{
		glBindImageTexture(unit, glID, 0, isLayered, 0, GL_READ_WRITE, internalFormat);
	}

	void Texture::Delete() const
	{
		glDeleteTextures(1, &glID);
	}
}