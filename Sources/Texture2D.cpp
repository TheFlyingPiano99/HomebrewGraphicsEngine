#include "Texture2D.h"
#include "MemoryManager.h"
#include <stb/stb_image.h>
#include "DebugUtils.h"


namespace Hogra {
	
	void Texture2D::Init(const std::filesystem::path& path, GLuint unit, GLenum _format, GLenum _pixelType, bool gammaCorrectionOnFloat)
	{
		this->format = _format;
		this->pixelType = _pixelType;

		// Stores the width, height, and the number of color channels of the image
		int widthImg, heightImg, numColCh;
		// Flips the image so it appears right side up
		stbi_set_flip_vertically_on_load(true);

		if (GL_FLOAT == pixelType && gammaCorrectionOnFloat) {
			stbi_ldr_to_hdr_gamma(2.2f);
		}
		else {
			stbi_ldr_to_hdr_gamma(1.0f);
		}

		bool is_hdr = stbi_is_hdr(path.string().c_str());
		if (!is_hdr && GL_FLOAT == pixelType) {
			DebugUtils::PrintWarning(
				"Texture2D", 
				std::string("Loading LDR image as GL_FLOAT texture from path:\n\"")
				.append(path.string()).append("\".").c_str()
			);
		}
		if (is_hdr && (GL_UNSIGNED_BYTE == pixelType || GL_BYTE == pixelType)) {
			DebugUtils::PrintWarning(
				"Texture2D",
				std::string("Loading HDR image as GL_BYTE or GL_UNSIGNED_BYTE texture from path:\n\"")
				.append(path.string()).append("\".").c_str()
			);
		}
		// Reads the image from a file and stores it in bytes
		unsigned char* imgBytes = 0;
		if (GL_FLOAT == pixelType) {
			imgBytes = (unsigned char*)stbi_loadf(path.string().c_str(), &widthImg, &heightImg, &numColCh, 0);
		}
		else {
			imgBytes = stbi_load(path.string().c_str(), &widthImg, &heightImg, &numColCh, 0);
		}
		if (nullptr == imgBytes) {	// Failed loading
			DebugUtils::PrintError("Texture2D", std::string("Couldn't load image file:\n\"").append(path.string()).append("\"!").c_str());
			this->Init(GL_RGBA, glm::ivec2(16, 16), unit, format, pixelType);
			return;
		}

		dimensions.x = widthImg;
		dimensions.y = heightImg;
		this->unit = unit;

		// Generates an OpenGL texture object
		glGenTextures(1, &glID);
		// Assigns the texture to a Texture Unit
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, glID);

		// Configures the type of algorithm that is used to make the image smaller or bigger
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Configures the way the texture repeats (if it does at all)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Extra lines in case you choose to use GL_CLAMP_TO_BORDER
		// float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
		// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

		auto internalFormat = GL_RGBA;
		switch (numColCh)
		{
		case 4: {internalFormat = (GL_FLOAT == _pixelType) ? GL_RGBA16F : GL_RGBA; break; }
		case 3: {internalFormat = (GL_FLOAT == _pixelType) ? GL_RGB16F : GL_RGB; break; }
		case 2: {internalFormat = (GL_FLOAT == _pixelType) ? GL_RG16F : GL_RG; break; }
		case 1: {internalFormat = (GL_FLOAT == _pixelType) ? GL_R16F : GL_R; break; }
		default: break;
		}
		// Assigns the image to the OpenGL Texture object
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, dimensions.x, dimensions.y, 0, format, pixelType, imgBytes);
		// Generates MipMaps
		glGenerateMipmap(GL_TEXTURE_2D);

		// Deletes the image data as it is already in the OpenGL Texture object
		stbi_image_free(imgBytes);

		// Unbinds the OpenGL Texture object so that it can't accidentally be modified
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture2D::Init(const std::vector<glm::vec4>& _bytes, glm::ivec2 _dimensions, GLuint _unit, GLenum _format, GLenum _pixelType)
	{
		this->bytes = _bytes;
		this->dimensions = _dimensions;
		this->unit = _unit;
		this->format = _format;
		this->pixelType = _pixelType;

		// Generates an OpenGL texture object
		glGenTextures(1, &glID);
		// Assigns the texture to a Texture Unit
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, glID);

		// Configures the type of algorithm that is used to make the image smaller or bigger
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Configures the way the texture repeats (if it does at all)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Assigns the image to the OpenGL Texture object
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, dimensions.x, dimensions.y, 0, format, pixelType, &bytes[0]);
		
		/*
		// Generates MipMaps
		glGenerateMipmap(GL_TEXTURE_2D);
		*/

		// Unbinds the OpenGL Texture object so that it can't accidentally be modified
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture2D::Init(const char* _buffer, glm::ivec2 _dimensions, GLuint _unit, GLenum _internalFormat, GLenum _format, GLenum _pixelType)
	{
		this->dimensions = _dimensions;
		this->unit = _unit;
		this->format = _format;
		this->pixelType = _pixelType;

		// Generates an OpenGL texture object
		glGenTextures(1, &glID);
		// Assigns the texture to a Texture Unit
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, glID);

		// Configures the type of algorithm that is used to make the image smaller or bigger
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Configures the way the texture repeats (if it does at all)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Assigns the image to the OpenGL Texture object
		glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, dimensions.x, dimensions.y, 0, format, pixelType, _buffer);

		/*
		// Generates MipMaps
			glGenerateMipmap(GL_TEXTURE_2D);
		*/

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture2D::Init(GLint _internalformat, glm::ivec2 _dimensions, GLuint _unit, GLenum _format, GLenum _pixelType, bool useMipmaps)
	{
		this->format = _format;
		this->pixelType = _pixelType;
		this->unit = _unit;

		// Generates an OpenGL texture object
		glGenTextures(1, &glID);
		// Assigns the texture to a Texture Unit
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, glID);
		this->dimensions = _dimensions;

		// Configures the type of algorithm that is used to make the image smaller or bigger
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Configures the way the texture repeats (if it does at all)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Assigns the image to the OpenGL Texture object
		glTexImage2D(GL_TEXTURE_2D, 0, _internalformat, dimensions.x, dimensions.y, 0, format, pixelType, nullptr);
		
		// Mipmap:
		if (useMipmaps) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		// Unbinds the OpenGL Texture object so that it can't accidentally be modified
		glBindTexture(GL_TEXTURE_2D, 0);
	}


	const glm::ivec2 Texture2D::GetDimensions() const {
		return dimensions;
	}

	void Texture2D::Bind() const
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, glID);
	}

	void Texture2D::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	
	const std::vector<glm::vec4>& Texture2D::GetBytes() const {
		return bytes;
	}

	void Texture2D::SetData(const std::vector<glm::vec4>& _data) {
		this->bytes = _data;
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, glID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, dimensions.x, dimensions.y, 0, format, pixelType, &bytes[0]);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture2D::SetFiltering(GLenum filtering) const
	{
		Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
	}

	void Texture2D::GenerateMipmap()
	{
		Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

}