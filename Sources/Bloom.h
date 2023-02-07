#pragma once
#include "PostProcessStage.h"
#include "UniformBuffer.h"
#include "ShaderProgram.h"
#include "VAO.h"
#include "VBO.h"
#include "FBO.h"


#define BLOOM_MIP_LEVELS 8


namespace Hogra {
	class Bloom : public PostProcessStage
	{
	public:
		Bloom();

		void Init(unsigned int width, unsigned int height);

		void Draw(const FBO& outFBO, const Texture2D& depthTexture, const Camera& camera) override;

		float getTreshold() const {
			return treshold;
		}

		void setTreshold(float _treshold) {
			treshold = _treshold;
		}
		
		void OnContextResize(unsigned int width, unsigned int height) override;

		void Bind() override;

	private:
		ShaderProgram prefilterProgram;
		ShaderProgram downSampleProgram;
		ShaderProgram upSampleProgram;
		ShaderProgram recombineProgram;
		Texture2D hdrTexture;
		Texture2D downScaledTextures[BLOOM_MIP_LEVELS];
		Geometry* quad;
		float treshold;
		float falloff;
		float mixBloom;
	};
}

