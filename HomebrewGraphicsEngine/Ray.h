#pragma once
#include "glm/glm.hpp"
namespace hograengine {

	class Ray
	{
	public:

		const glm::vec3& getPosition() const {
			return position;
		}

		void setPosition(const glm::vec3& pos) {
			position = pos;
		}

		const glm::vec3& getDirection() const {
			return direction;
		}

		void setDirection(const glm::vec3& dir) {
			direction = dir;
		}

	private:
		glm::vec3 position;
		glm::vec3 direction;
	};
}