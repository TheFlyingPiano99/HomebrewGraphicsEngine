#include "Plane.h"

namespace Hogra {

	Plane::Plane(glm::vec3 point, glm::vec3 norm) : point(point), normal(norm) {
	}

	void Plane::ExportData(ShaderProgram* shader, const char* uniform)
	{
		glUniform3f(glGetUniformLocation(shader->ID, std::string(uniform).append(".position").c_str()),
			point.x, point.y, point.z);
		glUniform3f(glGetUniformLocation(shader->ID, std::string(uniform).append(".normal").c_str()),
			normal.x, normal.y, normal.z);
	}
}