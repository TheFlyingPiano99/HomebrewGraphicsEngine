#include "Texture.h"
#include "glad/glad.h"

namespace Hogra {

	void Texture::Delete() const
	{
		glDeleteTextures(1, &glID);
	}
}