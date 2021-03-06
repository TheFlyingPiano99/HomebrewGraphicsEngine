#pragma once
#include "UserControl.h"
#include "Collider.h"

namespace Hogra {
	class FirstPersonControl : public UserControl
	{
	public:
		FirstPersonControl() {
			allowRotate = true;
		}

		void Update(float dt) override;

		void moveForward(float dt);
		void moveBackward(float dt);
		void moveLeft(float dt);
		void moveRight(float dt);
		void Rotate(float mouseX, float mouseY) override;
		
		void jump();

		void setCamera(Camera* cam) {
			camera = cam;
		}

		void setJumpImpulse(float j) {
			jumpImpulse = j;
		}

		void setJumpCollider(Collider* collider) {
			jumpCollider = collider;
		}

	private:
		bool isGrounded = false;
		float jumpCoolDown = 0.0f;
		Collider* jumpCollider = nullptr;
		float jumpImpulse = 1000.0f;
		Camera* camera = nullptr;
		float tSinceLastInput = 0.0f;
	};
}

