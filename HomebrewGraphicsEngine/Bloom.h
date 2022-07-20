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

		void BindBrightTexture();

		void UnbindBrightTexture();

		void draw(const FBO& fbo);

		float getTreshold() const {
			return treshold;
		}

		void setTreshold(float _treshold) {
			treshold = _treshold;
		}

		float getIntensity() const {
			return intensity;
		}

		void setIntensity(float _intensity) {
			intensity = _intensity;
		}
		
		void onResize(unsigned int width, unsigned int height);

	private:
		ShaderProgram* program = nullptr;
		UniformBufferObject* ubo = nullptr;
		unsigned int brightTexture = 0;
		VAO vao;
		VBO* vbo = nullptr;
		unsigned int pingpongFBO[2];
		unsigned int pingpongBuffers[2];
		float treshold;
		float intensity;
	};
}

