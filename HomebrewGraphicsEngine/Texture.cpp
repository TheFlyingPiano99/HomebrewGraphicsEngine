#include "Texture.h"
#include "glad/glad.h"

namespace hograengine {

	void Texture::Delete() const
	{
		glDeleteTextures(1, &ID);
	}
}