#include "Texture2D.h"

namespace hograengine {

	Texture2D::Texture2D(const std::string& path, GLuint unit, GLenum format, GLenum pixelType)
	{

		// Stores the width, height, and the number of color channels of the image
		int widthImg, heightImg, numColCh;
		// Flips the image so it appears right side up
		stbi_set_flip_vertically_on_load(true);
		// Reads the image from a file and stores it in bytes
		unsigned char* imgBytes = stbi_load(path.c_str(), &widthImg, &heightImg, &numColCh, 0);

		dimensions.x = widthImg;
		dimensions.y = heightImg;
		this->unit = unit;

		// Generates an OpenGL texture object
		glGenTextures(1, &ID);
		// Assigns the texture to a Texture Unit
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, ID);

		// Configures the type of algorithm that is used to make the image smaller or bigger
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Configures the way the texture repeats (if it does at all)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Extra lines in case you choose to use GL_CLAMP_TO_BORDER
		// float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
		// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

		// Assigns the image to the OpenGL Texture object
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimensions.x, dimensions.y, 0, format, pixelType, imgBytes);
		// Generates MipMaps
		glGenerateMipmap(GL_TEXTURE_2D);

		// Deletes the image data as it is already in the OpenGL Texture object
		stbi_image_free(imgBytes);

		// Unbinds the OpenGL Texture object so that it can't accidentally be modified
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture2D::Texture2D(std::vector<glm::vec4> bytes, glm::ivec2 dimensions, GLuint unit, GLenum format, GLenum pixelType)
		: bytes(bytes), dimensions(dimensions)
	{
		// Generates an OpenGL texture object
		glGenTextures(1, &ID);
		// Assigns the texture to a Texture Unit
		glActiveTexture(GL_TEXTURE0 + unit);
		this->unit = unit;
		glBindTexture(GL_TEXTURE_2D, ID);

		// Configures the type of algorithm that is used to make the image smaller or bigger
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Configures the way the texture repeats (if it does at all)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Assigns the image to the OpenGL Texture object
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimensions.x, dimensions.y, 0, format, pixelType, &bytes[0]);
		// Generates MipMaps
		glGenerateMipmap(GL_TEXTURE_2D);

		// Unbinds the OpenGL Texture object so that it can't accidentally be modified
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture2D::Texture2D(GLint internalformat, glm::ivec2 dimensions, GLuint unit, GLenum format, GLenum pixelType)
	{
		// Generates an OpenGL texture object
		glGenTextures(1, &ID);
		// Assigns the texture to a Texture Unit
		glActiveTexture(GL_TEXTURE0 + unit);
		this->unit = unit;
		glBindTexture(GL_TEXTURE_2D, ID);
		this->dimensions = dimensions;

		// Configures the type of algorithm that is used to make the image smaller or bigger
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Configures the way the texture repeats (if it does at all)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Assigns the image to the OpenGL Texture object
		glTexImage2D(GL_TEXTURE_2D, 0, internalformat, dimensions.x, dimensions.y, 0, format, pixelType, nullptr);
		/*
		glTexImage2D(GL_TEXTURE_2D, 1, internalformat, dimensions.x / 2, dimensions.y / 2, 0, format, pixelType, nullptr);
		glTexImage2D(GL_TEXTURE_2D, 2, internalformat, dimensions.x / 4, dimensions.y / 4, 0, format, pixelType, nullptr);
		glTexImage2D(GL_TEXTURE_2D, 3, internalformat, dimensions.x / 8, dimensions.y / 8, 0, format, pixelType, nullptr);
		glTexImage2D(GL_TEXTURE_2D, 4, internalformat, dimensions.x / 16, dimensions.y / 16, 0, format, pixelType, nullptr);
		glTexImage2D(GL_TEXTURE_2D, 5, internalformat, dimensions.x / 32, dimensions.y / 32, 0, format, pixelType, nullptr);
		glTexImage2D(GL_TEXTURE_2D, 6, internalformat, dimensions.x / 64, dimensions.y / 64, 0, format, pixelType, nullptr);
		glTexImage2D(GL_TEXTURE_2D, 7, internalformat, dimensions.x / 128, dimensions.y / 128, 0, format, pixelType, nullptr);
		glTexImage2D(GL_TEXTURE_2D, 8, internalformat, dimensions.x / 256, dimensions.y / 256, 0, format, pixelType, nullptr);
		glTexImage2D(GL_TEXTURE_2D, 9, internalformat, dimensions.x / 512, dimensions.y / 512, 0, format, pixelType, nullptr);
		glTexImage2D(GL_TEXTURE_2D, 10, internalformat, dimensions.x / 1024, dimensions.y / 1024, 0, format, pixelType, nullptr);
		*/

		// Unbinds the OpenGL Texture object so that it can't accidentally be modified
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	const glm::ivec2 Texture2D::getDimensions() const {
		return dimensions;
	}

	const std::vector<glm::vec4>& Texture2D::getBytes() const {
		return bytes;
	}


	glm::vec4& Texture2D::operator()(glm::ivec2 position)
	{
		if (position.x >= dimensions.x || position.x < 0
			|| position.y >= dimensions.y || position.y < 0) {
			return nullVector;
		}
		glm::vec4 v = bytes[position.y * dimensions.x + position.x];
		return v;
	}

	glm::vec4& Texture2D::operator()(glm::vec2 normalisedPosition) {
		return operator()(glm::ivec2(
			normalisedPosition.x * (dimensions.x - 1),
			normalisedPosition.y * (dimensions.y - 1)));
	}

	void Texture2D::Bind() const
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, ID);
	}

	void Texture2D::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}