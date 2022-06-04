#include "Texture.h"
#include "glad/glad.h"

void Texture::Delete() const
{
	glDeleteTextures(1, &ID);
}
