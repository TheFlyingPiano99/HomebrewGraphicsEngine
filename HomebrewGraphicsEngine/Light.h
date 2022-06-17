#pragma once
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include "ShaderProgram.h"

namespace hograengine {

	class Light {
	public:
		Light(glm::vec4 position, glm::vec3 powerDensity)
			: position(position), powerDensity(powerDensity) {}

		~Light() = default;

		void exportData(const ShaderProgram& program, unsigned int id) const;

	private:
		static float FOVRad;
		static float nearPlane;
		static float farPlane;

		glm::vec4 position;
		glm::vec3 powerDensity;
	};

}