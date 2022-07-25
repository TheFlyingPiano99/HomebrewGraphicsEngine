#pragma once
#include "UniformBuffer.h"
#include "ShaderProgram.h"
#include "VAO.h"
#include "VBO.h"
#include "FBO.h"

namespace hograengine {
	class Bloom
	{
	public:
		Bloom();

		~Bloom();

		void init(unsigned int width, unsigned int height);

		void draw(const FBO& outFBO);

		float getTreshold() const {
			return treshold;
		}

		void setTreshold(float _treshold) {
			treshold = _treshold;
		}
		
		void onResize(unsigned int width, unsigned int height);

		void Bind();

	private:
		ShaderProgram* prefilterProgram = nullptr;
		ShaderProgram* downSampleProgram = nullptr;
		ShaderProgram* upSampleProgram = nullptr;
		ShaderProgram* recombineProgram = nullptr;
		Texture2D* hdrTexture = nullptr;
		Texture2D* depthTexture = nullptr;
		std::vector<Texture2D*> downScaledTextures;
		VAO vao;
		VBO* vbo = nullptr;
		FBO fbo;
		float treshold;
		float falloff;
		float mixBloom;
	};
}

