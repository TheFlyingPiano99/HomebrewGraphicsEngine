#pragma once
#include "UserControl.h"

namespace hograengine {
	class FirstPersonControl : public UserControl
	{
	public:
		FirstPersonControl() {
			allowRotate = true;
		}

		void update(float dt) override;

		void moveForward(float dt);
		void moveBackward(float dt);
		void moveLeft(float dt);
		void moveRight(float dt);
		void rotate(float mouseX, float mouseY) override;
		
		void jump();

		void setCamera(Camera* cam) {
			camera = cam;
		}

		void setJumpImpulse(float j) {
			jumpImpulse = j;
		}


	private:
		float jumpImpulse = 1000.0f;
		Camera* camera = nullptr;
		float tSinceLastInput = 0.0f;
	};
}

