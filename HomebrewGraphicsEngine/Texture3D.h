#pragma once
#include "Texture.h"

#include<glad/glad.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include <vector>

#include"ShaderProgram.h"
namespace hograengine {

	struct Dimensions {
		int width, height, depth, bytesPerVoxel;
		float widthScale, heightScale, depthScale = 1.0f;
	};

	class Texture3D : public Texture
	{
	public:
		float shininess = 16.0f;	// Should be extracted to Material
		std::vector<char> bytes;
		const Dimensions dimensions;
		int maxValue;
		Texture3D(const char* directory, const Dimensions dimensions, GLuint slot, GLenum format);
		~Texture3D();

		const Dimensions& getDimensions() {
			return dimensions;
		}

		const float operator()(glm::ivec3 position);

		const glm::vec4 resampleGradientAndDensity(glm::ivec3 position);


		// Inherited via Texture
		virtual void Bind() const override;

		virtual void Unbind() const override;

	};
}