#pragma once
#include<glm/glm.hpp>
#include "ShaderProgram.h"
namespace Hogra {

	class Plane
	{
		glm::vec3 point;
		glm::vec3 normal;
	public:
		Plane(glm::vec3 point, glm::vec3 norm);

		void ExportData(ShaderProgram* shader, const char* uniform);


		glm::vec3 getPoint() {
			return point;
		}

		glm::vec3 getNormal() {
			return normal;
		}

		void setPoint(const glm::vec3 p) {
			point = p;
		}

		void setNormal(const glm::vec3 n) {
			normal = n;
		}
	};

}