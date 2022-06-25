#pragma once
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include "ShaderProgram.h"
#include "UniformBuffer.h"

namespace hograengine {

	class Light {
	public:
		Light(glm::vec4 position, glm::vec3 powerDensity)
			: position(position), powerDensity(powerDensity) {}

		~Light() = default;

		void exportData(UniformBufferObject* ubo, unsigned int& idx);

	private:
		glm::vec4 position;
		glm::vec3 powerDensity;

	};

}