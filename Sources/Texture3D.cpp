#include "Texture3D.h"
#include <iomanip>
#include <sstream>
#include "MemoryManager.h"
#include <fstream>
#include <filesystem>


namespace Hogra {
	
	void Texture3D::Init(const std::string& directory, GLuint slot, GLenum format)
	{
		std::string name;	// Discarded !!!
		if (!ReadDimensions(std::string(directory).append("/dimensions.txt").c_str(), name, this->dimensions)) {
			throw std::exception("Failed to read dimensions of voxel data!");
		}
		bool swapBytes = false;
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
		maxValue = (int)std::powf(2.0f, (float)dimensions.bytesPerVoxel * 8.0f) - 1;

		// Flips the image so it appears right side up
		//stbi_set_flip_vertically_on_load(false);

		// Reads the image from a file and stores it in bytes
		for (int z = 0; z < dimensions.depth; z++) {
			std::stringstream pathss;
			pathss << directory << "/" << std::setw(3) << std::setfill('0') << z + 1 << ".tif";	// <directory/><number of image>.tif
			std::string path = pathss.str();
			FILE* file;
			errno_t err;
			if (err = fopen_s(&file, path.c_str(), "rb") == 0) {
				unsigned char* header = new unsigned char[headerSize];
				int headerByteCount = fread(header, sizeof(char), headerSize, file);
				int bytesCount = fread(
					&bytes[0] + z * dimensions.width * dimensions.height * dimensions.bytesPerVoxel,
					sizeof(char),
					dimensions.width * dimensions.height * dimensions.bytesPerVoxel,
					file
				);
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
		glGenTextures(1, &glID);
		// Assigns the texture to a Texture Unit
		glActiveTexture(GL_TEXTURE0 + slot);
		unit = slot;
		glBindTexture(GL_TEXTURE_3D, glID);

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

	void Texture3D::Init(glm::ivec3 resolution, std::function<float(float, float, float)> func, GLuint slot, GLenum format)
	{
		dimensions.width = resolution.x;
		dimensions.height = resolution.y;
		dimensions.depth = resolution.z;
		dimensions.bytesPerVoxel = 1;

		bytes.resize(resolution.x * resolution.y * resolution.z);

		for (int x = 0; x < resolution.x; x++) {
			for (int y = 0; y < resolution.y; y++) {
				for (int z = 0; z < resolution.z; z++) {
					unsigned int idx = z * dimensions.height * dimensions.width * dimensions.bytesPerVoxel + y * dimensions.width * dimensions.bytesPerVoxel + x * dimensions.bytesPerVoxel;
					float f = func((float)x / (float)resolution.x, (float)y / (float)resolution.y, (float)z / (float)resolution.z);
					char v = 255 * f;
					if (f > 0.01f) {
						v = 200;
					}
					else {
						v = 0;
					}
					bytes[idx] = v;
				}
			}
		}
		// Generates an OpenGL texture object
		glGenTextures(1, &glID);
		// Assigns the texture to a Texture Unit
		glActiveTexture(GL_TEXTURE0 + slot);
		unit = slot;
		glBindTexture(GL_TEXTURE_3D, glID);

		// Configures the type of algorithm that is used to make the image smaller or bigger
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Configures the way the texture repeats (if it does at all)
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

		// Assigns the image to the OpenGL Texture object
		//TODO
		glTexImage3D(GL_TEXTURE_3D, 0, format, dimensions.width, dimensions.height, dimensions.depth, 0, format, GL_UNSIGNED_BYTE, &bytes[0]);
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
		glBindTexture(GL_TEXTURE_3D, glID);
	}

	void Texture3D::Unbind() const
	{
		glBindTexture(GL_TEXTURE_3D, 0);
	}

	const Dimensions& Texture3D::GetDimensions() const {
		return dimensions;
	}

	const glm::vec4 Texture3D::ResampleGradientAndDensity(glm::ivec3 position) const
	{
		float intensity = this->operator()(position);
		glm::vec3 stepSize = glm::vec3(1.0);
		glm::vec3 sample0, sample1;
		sample0.x = operator()(glm::ivec3(position.x - stepSize.x, position.y, position.z));
		sample1.x = operator()(glm::ivec3(position.x + stepSize.x, position.y, position.z));
		sample0.y = operator()(glm::ivec3(position.x, position.y - stepSize.y, position.z));
		sample1.y = operator()(glm::ivec3(position.x, position.y + stepSize.y, position.z));
		sample0.z = operator()(glm::ivec3(position.x, position.y, position.z - stepSize.z));
		sample1.z = operator()(glm::ivec3(position.x, position.y, position.z + stepSize.z));
		glm::vec3 scaledPosition = glm::vec3(position) - glm::vec3(0.5f);
		glm::vec3 fraction = scaledPosition - glm::floor(scaledPosition);
		glm::vec3 correctionPolynomial = (fraction * (fraction - glm::vec3(1.0f))) / glm::vec3(2.0f);
		intensity += glm::dot((sample0 - intensity * 2.0f + sample1),
			correctionPolynomial);
		return glm::vec4(sample1 - sample0, intensity);
	}

	float Texture3D::operator()(glm::ivec3 position) const
	{
		if (position.x >= dimensions.width
			|| position.x < 0
			|| position.y >= dimensions.height
			|| position.y < 0
			|| position.z >= dimensions.depth
			|| position.z < 0) {
			return 0.0f;
		}
		int idx = 
			position.z * dimensions.height * dimensions.width * dimensions.bytesPerVoxel 
			+ position.y * dimensions.width * dimensions.bytesPerVoxel 
			+ position.x * dimensions.bytesPerVoxel;

		float result;
		if (dimensions.bytesPerVoxel == 1) {
			result = (float)bytes[idx] / (float)maxValue;
		}
		else if (dimensions.bytesPerVoxel == 2) {
			char a = bytes[idx];
			char b = bytes[idx + 1];
			short c = (((short)a) << 8) | b;
			result = (float)c / (float)maxValue;
		}
		return result;
	}
	
	bool Texture3D::ReadDimensions(const char* path, std::string& name, Dimensions& dimensions)
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

	void Texture3D::SetFiltering(GLenum filtering) const
	{
		Bind();
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filtering);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filtering);
		Unbind();
	}

}