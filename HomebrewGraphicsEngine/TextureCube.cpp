#include "TextureCube.h"
#include "MemoryManager.h"

namespace Hogra {
	
	TextureCube* TextureCube::Instantiate() {
		auto* instance = new TextureCube();
		heapAllocatedInstances.push_back(instance);
		return instance;
	}
	void TextureCube::Init(std::vector<std::string>& images, GLuint unit)
	{
		glGenTextures(1, &ID);
		glActiveTexture(GL_TEXTURE0 + unit);
		this->unit = unit;
		glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

		// Stores the width, height, and the number of color channels of the image
		int widthImg, heightImg, numColCh;
		// Flips the image so it appears right side up
		//stbi_set_flip_vertically_on_load(true);

		stbi_set_flip_vertically_on_load(false);
		for (int i = 0; i < 6; i++) {
			unsigned char* imgBytes = stbi_load(images[i].c_str(), &widthImg, &heightImg, &numColCh, 0);
			if (imgBytes == nullptr) {
				throw std::exception("Failed to load texture cube!");
			}
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, widthImg, heightImg, 0, GL_RGB, GL_UNSIGNED_BYTE, imgBytes);
			stbi_image_free(imgBytes);
		}

		dimensions.x = widthImg;
		dimensions.y = heightImg;

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	}

	void TextureCube::Bind()
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	}

	void TextureCube::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void TextureCube::Delete()
	{
		glDeleteTextures(1, &ID);
	}

	void TextureCube::Bind() const
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	}

	void TextureCube::Unbind() const
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
}