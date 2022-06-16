#pragma once
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include "ShaderProgram.h"
namespace hograengine {

	class Light {
	public:
		Light(glm::vec4 position, glm::vec3 powerDensity, bool isShadowCaster = false)
			: position(position), powerDensity(powerDensity), isShadowCaster(isShadowCaster) {}

		void exportData(const ShaderProgram& program, unsigned int id) const;

		void updateMatrix(glm::vec3 lookAt);

	private:
		static float FOVRad;
		static float nearPlane;
		static float farPlane;

		glm::vec4 position;
		glm::vec3 powerDensity;
		glm::mat4 viewProjMatrix = glm::mat4(1.0f);
		bool isShadowCaster;
	};

}