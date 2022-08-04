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
		Dimensions dimensions;
		
		void Init(const char* directory, const Dimensions dimensions, GLuint slot, GLenum format);
		
		~Texture3D() override;

		const Dimensions& getDimensions() {
			return dimensions;
		}

		// Inherited via Texture
		void Bind() const override;

		void Unbind() const override;

	};
}