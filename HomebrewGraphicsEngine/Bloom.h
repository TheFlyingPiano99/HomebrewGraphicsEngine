#pragma once
#include "UniformBuffer.h"
#include "ShaderProgram.h"
#include "VAO.h"
#include "VBO.h"
#include "FBO.h"

#define BLOOM_MIP_LEVELS 8


namespace Hogra {
	class Bloom
	{
	public:
		Bloom();

		~Bloom();

		void Init(unsigned int width, unsigned int height);

		void Draw(const FBO& outFBO);

		float getTreshold() const {
			return treshold;
		}

		void setTreshold(float _treshold) {
			treshold = _treshold;
		}
		
		void onResize(unsigned int width, unsigned int height);

		void Bind();

	private:
		ShaderProgram prefilterProgram;
		ShaderProgram downSampleProgram;
		ShaderProgram upSampleProgram;
		ShaderProgram recombineProgram;
		Texture2D hdrTexture;
		Texture2D depthTexture;
		Texture2D downScaledTextures[BLOOM_MIP_LEVELS];
		VAO vao;
		VBO* vbo = nullptr;
		FBO fbo;
		float treshold;
		float falloff;
		float mixBloom;
	};
}

