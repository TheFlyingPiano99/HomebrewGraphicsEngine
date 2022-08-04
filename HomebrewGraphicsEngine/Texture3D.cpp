#include "Texture3D.h"
#include <iomanip>
#include <sstream>
#include "MemoryManager.h"

namespace Hogra {
	
	Texture3D* Texture3D::Instantiate() {
		auto* instance = new Texture3D();
		MemoryManager::heapAllocatedInstances.push_back(instance);
		return instance;
	}
	void Texture3D::Init(const char* directory, const Dimensions dimensions, GLuint slot, GLenum format)
	{
		this->dimensions = dimensions;
		bool swapBytes = true;
		unsigned int headerSize = 0;
		GLenum pixelType;
		if (dimensions.bytesPerVoxel == 1) {
			pixelType = GL_UNSIGNED_BYTE;
		}
		else if (dimensions.bytesPerVoxel == 2) {
			pixelType = GL_HALF_FLOAT;
		}
		else if (dimensions.bytesPerVoxel == 4) {
			pixelType = GL_FLOAT;
		}

		bytes = std::vector<char>(dimensions.width * dimensions.height * dimensions.depth * dimensions.bytesPerVoxel);

		// Flips the image so it appears right side up
		//stbi_set_flip_vertically_on_load(false);

		// Reads the image from a file and stores it in bytes
		for (int z = 0; z < dimensions.depth; z++) {
			std::stringstream pathss;
			pathss << directory << std::setw(3) << std::setfill('0') << z + 1 << ".tif";	// <directory/><number of image>.tif
			std::string path = pathss.str();
			FILE* file;
			errno_t err;
			if (err = fopen_s(&file, path.c_str(), "rb") == 0) {
				unsigned char* header = new unsigned char[headerSize];
				int headerByteCount = fread(header, sizeof(char), headerSize, file);
				int bytesCount = fread(&bytes[0] + z * dimensions.width * dimensions.height * dimensions.bytesPerVoxel, sizeof(char), dimensions.width * dimensions.height * dimensions.bytesPerVoxel, file);
				fclose(file);
				delete[] header;
			}
		}

		if (swapBytes) {
			for (int i = 0; i < dimensions.width * dimensions.height * dimensions.depth; i++) {
				for (int j = 0; j < dimensions.bytesPerVoxel / 2; j++) {
					char temp = bytes[i * dimensions.bytesPerVoxel + j];
					bytes[i * dimensions.bytesPerVoxel + j] = bytes[i * dimensions.bytesPerVoxel + dimensions.bytesPerVoxel - 1 - j];
					bytes[i * dimensions.bytesPerVoxel + dimensions.bytesPerVoxel - 1 - j] = temp;
				}
			}
		}

		// Generates an OpenGL texture object
		glGenTextures(1, &ID);
		// Assigns the texture to a Texture Unit
		glActiveTexture(GL_TEXTURE0 + slot);
		unit = slot;
		glBindTexture(GL_TEXTURE_3D, ID);

		// Configures the type of algorithm that is used to make the image smaller or bigger
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Configures the way the texture repeats (if it does at all)
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

		// Assigns the image to the OpenGL Texture object
		//TODO
		glTexImage3D(GL_TEXTURE_3D, 0, format, dimensions.width, dimensions.height, dimensions.depth, 0, format, pixelType, &bytes[0]);
		// Generates MipMaps
		glGenerateMipmap(GL_TEXTURE_3D);

		// Unbinds the OpenGL Texture object so that it can't accidentally be modified
		glBindTexture(GL_TEXTURE_3D, 0);
	}

	Texture3D::~Texture3D()
	{
		this->Delete();
	}

	void Texture3D::Bind() const
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_3D, ID);
	}

	void Texture3D::Unbind() const
	{
		glBindTexture(GL_TEXTURE_3D, 0);
	}
}