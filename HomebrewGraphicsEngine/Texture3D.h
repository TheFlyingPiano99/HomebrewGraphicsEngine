#pragma once
#include "Texture.h"

#include<glad/glad.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include <vector>

#include"ShaderProgram.h"
namespace Hogra {

	struct Dimensions {
		int width, height, depth, bytesPerVoxel;
		float widthScale, heightScale, depthScale = 1.0f;
	};

	class Texture3D : public Texture
	{
	public:

		static Texture3D* Instantiate();

		std::vector<char> bytes;
		
		void Init(const std::string& directory, GLuint slot, GLenum format);
		
		~Texture3D() override;

		// Inherited via Texture
		void Bind() const override;

		void Unbind() const override;

		const Dimensions& GetDimensions() const;

		const glm::vec4 ResampleGradientAndDensity(glm::ivec3 position);

		const float operator()(glm::ivec3 position);

	private:
		bool ReadDimensions(const char* path, std::string& name, Dimensions& dimensions);
		Dimensions dimensions;
		int maxValue = 256;
	};
}