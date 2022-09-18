#include "Texture3D.h"
#include <iomanip>
#include <sstream>
#include "MemoryManager.h"
#include <fstream>
#include <filesystem>


namespace Hogra {
	
	Texture3D* Texture3D::Instantiate() {
		auto* instance = new Texture3D();
		heapAllocatedInstances.push_back(instance);
		return instance;
	}
	void Texture3D::Init(const std::string& directory, GLuint slot, GLenum format)
	{
		std::string name;	// Discarded !!!
		if (!readDimensions(std::string(directory).append("/dimensions.txt").c_str(), name, this->dimensions)) {
			throw new std::exception("Failed to read dimensions of voxel data!");
		}
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
			pathss << directory << "/" << std::setw(3) << std::setfill('0') << z + 1 << ".bmp";	// <directory/><number of image>.tif
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

	const Dimensions& Texture3D::GetDimensions() const {
		return dimensions;
	}
	
	bool Texture3D::readDimensions(const char* path, std::string& name, Dimensions& dimensions)
	{
		std::ifstream dimesionsStream;
		std::string line;
		std::string token;
		std::vector<std::string> tokens;
		dimesionsStream.open(path);
		if (!dimesionsStream) {
			return false;
		}
		else {
			while (std::getline(dimesionsStream, line)) {
				std::stringstream lineStream = std::stringstream(line);
				while (std::getline(lineStream, token, ' ')) {
					tokens.push_back(token);
					if (tokens.size() >= 3 && tokens[tokens.size() - 2] == "=") {
						if (tokens[tokens.size() - 3] == "name") {
							name = tokens[tokens.size() - 1];
						}
						else if (tokens[tokens.size() - 3] == "width") {
							dimensions.width = std::stoi(tokens[tokens.size() - 1]);
						}
						else if (tokens[tokens.size() - 3] == "height") {
							dimensions.height = std::stoi(tokens[tokens.size() - 1]);
						}
						else if (tokens[tokens.size() - 3] == "depth") {
							dimensions.depth = std::stoi(tokens[tokens.size() - 1]);
						}
						else if (tokens[tokens.size() - 3] == "bytesPerVoxel") {
							dimensions.bytesPerVoxel = std::stoi(tokens[tokens.size() - 1]);
						}
						else if (tokens[tokens.size() - 3] == "widthScale") {
							dimensions.widthScale = std::stof(tokens[tokens.size() - 1]);
						}
						else if (tokens[tokens.size() - 3] == "heightScale") {
							dimensions.heightScale = std::stof(tokens[tokens.size() - 1]);
						}
						else if (tokens[tokens.size() - 3] == "depthScale") {
							dimensions.depthScale = std::stof(tokens[tokens.size() - 1]);
						}
					}
				}
				tokens.clear();
			}
		}
		return true;
	}
}