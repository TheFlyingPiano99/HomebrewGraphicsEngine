#include "Texture2D.h"
#include "MemoryManager.h"
#include <stb/stb_image.h>
#include "DebugUtils.h"


namespace Hogra {
	
	std::vector<Texture2D*> Texture2D::allTexture2Ds = std::vector<Texture2D*>();

	void Texture2D::Init(const std::filesystem::path& path, GLuint _unit, GLenum _format, GLenum _pixelType, bool gammaCorrectionOnFloat, bool useMipMaps)
	{
		this->unit = _unit;
		this->clientDataFormat = _format;
		this->clientDataType = _pixelType;

		allTexture2Ds.push_back(this);


		// Stores the width, height, and the number of color channels of the image
		int widthImg, heightImg, numColCh;
		// Flips the image so it appears right side up
		stbi_set_flip_vertically_on_load(true);

		if (GL_FLOAT == clientDataType && gammaCorrectionOnFloat) {
			stbi_ldr_to_hdr_gamma(2.2f);
		}
		else {
			stbi_ldr_to_hdr_gamma(1.0f);
		}

		bool is_hdr = stbi_is_hdr(path.string().c_str());
		if (!is_hdr && GL_FLOAT == clientDataType) {
			DebugUtils::PrintWarning(
				"Texture2D", 
				std::string("Loading LDR image as GL_FLOAT texture from path:\n\"")
				.append(path.string()).append("\".").c_str()
			);
		}
		if (is_hdr && (GL_UNSIGNED_BYTE == clientDataType || GL_BYTE == clientDataType)) {
			DebugUtils::PrintWarning(
				"Texture2D",
				std::string("Loading HDR image as GL_BYTE or GL_UNSIGNED_BYTE texture from path:\n\"")
				.append(path.string()).append("\".").c_str()
			);
		}
		// Reads the image from a file and stores it in bytes
		unsigned char* imgBytes = 0;
		if (GL_FLOAT == clientDataType) {
			imgBytes = (unsigned char*)stbi_loadf(path.string().c_str(), &widthImg, &heightImg, &numColCh, 0);
		}
		else {
			imgBytes = stbi_load(path.string().c_str(), &widthImg, &heightImg, &numColCh, 0);
		}
		if (nullptr == imgBytes) {	// Failed loading
			DebugUtils::PrintError("Texture2D", std::string("Couldn't load image file:\n\"").append(path.string()).append("\"!").c_str());
			this->Init(glm::ivec2(16, 16), unit, GL_RGBA8, clientDataFormat, clientDataType);
			return;
		}

		dimensions.x = widthImg;
		dimensions.y = heightImg;

		// Generates an OpenGL texture object
		glGenTextures(1, &glID);
		// Assigns the texture to a Texture Unit
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, glID);

		// Configures the way the texture repeats (if it does at all)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		internalFormat = GL_RGBA;
		switch (numColCh)
		{
		case 4: {internalFormat = (GL_FLOAT == _pixelType) ? GL_RGBA16F : GL_RGBA; break; }
		case 3: {internalFormat = (GL_FLOAT == _pixelType) ? GL_RGB16F : GL_RGB; break; }
		case 2: {internalFormat = (GL_FLOAT == _pixelType) ? GL_RG16F : GL_RG; break; }
		case 1: {internalFormat = (GL_FLOAT == _pixelType) ? GL_R16F : GL_R; break; }
		default: break;
		}
		// Assigns the image to the OpenGL Texture object
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, dimensions.x, dimensions.y, 0, clientDataFormat, clientDataType, imgBytes);

		// Deletes the image data as it is already in the OpenGL Texture object
		stbi_image_free(imgBytes);

		// Filtering:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Mipmap:
		if (useMipMaps) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glGenerateMipmap(GL_TEXTURE_2D);
		}		


		// Unbinds the OpenGL Texture object so that it can't accidentally be modified
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture2D::Init(const std::vector<glm::vec4>& _bytes, glm::ivec2 _dimensions, GLuint _unit, GLenum _format, GLenum _pixelType, bool useMipmaps)
	{
		this->bytes = _bytes;
		this->dimensions = _dimensions;
		this->unit = _unit;
		this->clientDataFormat = _format;
		this->clientDataType = _pixelType;
		this->internalFormat = GL_RGBA16;

		allTexture2Ds.push_back(this);

		// Generates an OpenGL texture object
		glGenTextures(1, &glID);
		// Assigns the texture to a Texture Unit
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, glID);

		// Assigns the image to the OpenGL Texture object
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, dimensions.x, dimensions.y, 0, clientDataFormat, clientDataType, &bytes[0]);
		
		// Configures the type of algorithm that is used to make the image smaller or bigger
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Configures the way the texture repeats (if it does at all)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Mipmap:
		if (useMipmaps) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		// Unbinds the OpenGL Texture object so that it can't accidentally be modified
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture2D::Init(const char* _buffer, glm::ivec2 _dimensions, GLuint _unit, GLenum _internalFormat, GLenum _format, GLenum _pixelType)
	{
		this->dimensions = _dimensions;
		this->unit = _unit;
		this->internalFormat = _internalFormat;
		this->clientDataFormat = _format;
		this->clientDataType = _pixelType;

		allTexture2Ds.push_back(this);

		// Generates an OpenGL texture object
		glGenTextures(1, &glID);
		// Assigns the texture to a Texture Unit
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, glID);

		// Assigns the image to the OpenGL Texture object
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, dimensions.x, dimensions.y, 0, clientDataFormat, clientDataType, _buffer);

		// Configures the type of algorithm that is used to make the image smaller or bigger
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Configures the way the texture repeats (if it does at all)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		/*
		// Generates MipMaps
			glGenerateMipmap(GL_TEXTURE_2D);
		*/

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture2D::Init(glm::ivec2 _dimensions, GLuint _unit, GLint _internalformat, GLenum _format, GLenum _pixelType, bool useMipmaps)
	{
		this->unit = _unit;
		this->internalFormat = _internalformat;
		this->clientDataFormat = _format;
		this->clientDataType = _pixelType;
		
		allTexture2Ds.push_back(this);

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
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, dimensions.x, dimensions.y, 0, clientDataFormat, clientDataType, nullptr);
		
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

	void Texture2D::SetFiltering(GLenum filtering) const
	{
		Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
	}

	void Texture2D::GenerateMipmap() const
	{
		Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void Texture2D::WriteData(void* dataPtr)
	{
		glBindTexture(GL_TEXTURE_2D, glID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
			dimensions.x, dimensions.y,
			clientDataFormat, clientDataType, dataPtr);
	}

	void Texture2D::ReadData(void* dataPtr) const
	{
		glBindTexture(GL_TEXTURE_2D, glID);
		glGetTexImage(GL_TEXTURE_2D, 0, clientDataFormat, clientDataType, dataPtr);
	}

	void Texture2D::SaveToPPM(const std::filesystem::path& path) {
		auto dim = GetDimensions();

		unsigned int channels = GetNumberOfClientSideChannels();
		unsigned int bytesPerChannel = GetNumberOfClientSideBytesPerChannel();

		if (0 == bytesPerChannel || 0 == channels) {	// Failed to determine type or format
			return;
		}

		auto* pixels = new unsigned char[dim.x * dim.y * channels * bytesPerChannel];
		ReadData(pixels);

		FILE* output_image;
		auto errn = fopen_s(&output_image, path.string().c_str(), "wt");
		fprintf(output_image, "P3\n");
		fprintf(output_image, "# Created with Hogra engine\n");
		fprintf(output_image, "%d %d\n", dim.x, dim.y);
		fprintf(output_image, "255\n");

		for (int x = 0; x < dim.x; x++)
		{
			for (int y = 0; y < dim.y; y++)
			{
				int baseIdx = x * dim.y * channels * bytesPerChannel + y * channels * bytesPerChannel;
				fprintf(output_image,
					"%u %u %u ",
					(unsigned int)pixels[baseIdx],
					(1 < channels) ? (unsigned int)pixels[baseIdx + 1 * bytesPerChannel] : 0,
					(2 < channels) ? (unsigned int)pixels[baseIdx + 2 * bytesPerChannel] : 0
				);
			}
			fprintf(output_image, "\n");
		}
		fclose(output_image);
		delete[] pixels;
	}

	void Texture2D::SaveAllToPPM()
	{
		auto savesPath = AssetFolderPathManager::getInstance()->getSavesFolderPath();
		int i = 0;
		for (auto texture : allTexture2Ds) {
			texture->SaveToPPM(savesPath.string().append("texture").append(std::to_string(i)).append(".ppm"));
			i++;
		}
	}
}

