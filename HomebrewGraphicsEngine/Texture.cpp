#include "Texture.h"
#include "glad/glad.h"

namespace Hogra {

	std::vector<Texture*> Texture::heapAllocatedInstances = std::vector<Texture*>();

	void Texture::Delete() const
	{
		glDeleteTextures(1, &ID);
	}
}