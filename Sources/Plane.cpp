#include "Plane.h"

namespace Hogra {

	Plane::Plane(glm::vec3 point, glm::vec3 norm) : point(point), normal(norm) {
	}

	void Plane::ExportData(ShaderProgram* shader, const char* uniform)
	{
		shader->SetUniform(std::string(uniform).append(".position").c_str(), point);
		shader->SetUniform(std::string(uniform).append(".normal").c_str(), normal);
	}
}