#include "Light.h"
#include<glm/gtc/matrix_transform.hpp>
#include "Scene.h"
namespace hograengine {

	float Light::FOVRad = glm::radians(45.0f);
	float Light::nearPlane = 0.1f;
	float Light::farPlane = 1000.0f;

	void Light::exportData(const ShaderProgram& program, unsigned int id) const {
		std::string structName("lights[");
		structName.append(std::to_string(id)).append("]");
		std::string positionName(structName);
		positionName.append(".position");
		glUniform4f(glGetUniformLocation(program.ID, positionName.c_str()), position.x, position.y, position.z, position.w);

		std::string powerDensityName(structName);
		powerDensityName.append(".powerDensity");
		glUniform3f(glGetUniformLocation(program.ID, powerDensityName.c_str()), powerDensity.r, powerDensity.g, powerDensity.b);
	}
}