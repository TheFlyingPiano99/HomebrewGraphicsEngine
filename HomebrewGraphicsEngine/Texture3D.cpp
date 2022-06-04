#include "Texture3D.h"
#include <iomanip>
#include <sstream>

Texture3D::Texture3D(const char* directory, const Dimensions dimensions, GLuint slot, GLenum format)
	: dimensions(dimensions), maxValue(std::pow(2, dimensions.bytesPerVoxel * 8) - 1)
{
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

const glm::vec4 Texture3D::resampleGradientAndDensity(glm::ivec3 position)
{
	float intensity = operator()(position);
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

const float Texture3D::operator()(glm::ivec3 position)
{
	if (position.x >= dimensions.width
		|| position.x < 0
		|| position.y >= dimensions.height
		|| position.y < 0
		|| position.z >= dimensions.depth
		|| position.z < 0) {
		return 0.0f;	// Prevent access violation error.
	}
	int idx = position.z * dimensions.height * dimensions.width * dimensions.bytesPerVoxel + position.y * dimensions.width * dimensions.bytesPerVoxel + position.x * dimensions.bytesPerVoxel;
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

void Texture3D::Bind() const
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_3D, ID);
}

void Texture3D::Unbind() const
{
	glBindTexture(GL_TEXTURE_3D, 0);
}