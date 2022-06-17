#pragma once
#include "Component.h"
#include <glm/ext/vector_float3.hpp>
#include "Physics.h"

namespace hograengine {
	class UserControl : public Component
	{
	public:
		virtual void moveForward(float dt);
		virtual void moveBackward(float dt);
		virtual void moveLeft(float dt);
		virtual void moveRight(float dt);
		virtual void moveUp(float dt);
		virtual void moveDown(float dt);
		virtual void rotate(float mouseX, float mouseY);

		void setInitialDirection(const glm::vec3& dir) {
			initialDirection = dir;
		}

		void setInitialUp(const glm::vec3& _up) {
			initialUp = _up;
		}

		void setPropellingForce(const glm::vec3& f) {
			propellingForce = f;
		}

		void setRotatingTorque(const glm::vec3& tau) {
			rotatingTorque = tau;
		}

		void setPhysics(Physics* _physics) {
			physics = _physics;
		}

	protected:

		glm::vec3 initialDirection;
		glm::vec3 initialUp;

		glm::vec3 ahead;
		glm::vec3 up;
		glm::vec3 right;

		bool allowMove = true;
		glm::vec3 propellingForce;
		bool allowRotate = false;
		bool directRotation = true;
		glm::vec3 rotatingTorque;

		Physics* physics;

		// Inherited via Component
		void control(float dt) override;
		void update(float dt) override;
	};
}

