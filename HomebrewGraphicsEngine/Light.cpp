#include "Light.h"
#include<glm/gtc/matrix_transform.hpp>
#include "Scene.h"

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

	if (isShadowCaster) {
		std::string viewProjName(structName);
		viewProjName.append(".viewProjMatrix");
		glUniformMatrix4fv(glGetUniformLocation(program.ID, viewProjName.c_str()), 1, GL_FALSE, glm::value_ptr(viewProjMatrix));
	}
}

void Light::updateMatrix(glm::vec3 lookAt)
{
	if (!isShadowCaster) {
		return;
	}
	viewProjMatrix 
		= glm::perspective(FOVRad, 1.0f, nearPlane, farPlane)
		* glm::lookAt(glm::vec3(position), lookAt, Scene::getInstance()->getPreferedUp());
}
