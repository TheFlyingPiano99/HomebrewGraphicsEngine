#pragma once
#include "Component.h"
#include <glm/ext/vector_float3.hpp>
#include "Physics.h"

namespace Hogra {
	class UserControl : public Component
	{
	public:
		virtual ~UserControl() = default;

		virtual void MoveForward();
		virtual void MoveBackward();
		virtual void MoveLeft();
		virtual void MoveRight();
		virtual void MoveUp();
		virtual void MoveDown();
		virtual void Rotate(const glm::vec2& delta);
		virtual void Zoom(float delta);
		virtual void primaryAction();
		virtual void secondaryAction();
		virtual void grab();
		virtual void release();

		void setInitialDirection(const glm::vec3& dir) {
			initialDirection = dir;
		}

		void setInitialUp(const glm::vec3& _up) {
			initialUp = _up;
		}

		void setPropellingForce(float f) {
			propellingForce = f;
		}

		void setRotatingTorque(const glm::vec3& tau) {
			rotatingTorque = tau;
		}

		void setPhysics(Physics* _physics) {
			physics = _physics;
		}

		void SetPositionProvider(PositionProvider* provider) {
			positionProvider = provider;
		}

		void SetOrientationProvider(OrientationProvider* provider) {
			orientationProvider = provider;
		}

	protected:

		glm::vec3 initialDirection;
		glm::vec3 initialUp;

		glm::vec3 ahead;
		glm::vec3 up;
		glm::vec3 right;

		bool allowMove = true;
		float propellingForce;
		bool allowRotate = false;
		bool directRotation = true;
		glm::vec3 rotatingTorque;

		Physics* physics;

		// Inherited via Component
		void EarlyPhysicsUpdate(float dt) override;
		void LatePhysicsUpdate(float dt) override;

		PositionProvider* positionProvider = nullptr;
		OrientationProvider* orientationProvider = nullptr;
	};
}

