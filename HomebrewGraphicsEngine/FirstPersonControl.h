#pragma once
#include "UserControl.h"
#include "Collider.h"
#include "Scene.h"

namespace Hogra {
	class FirstPersonControl : public UserControl
	{
	public:

		static FirstPersonControl* Instantiate();

		FirstPersonControl() {
			allowRotate = true;
		}

		~FirstPersonControl() override;

		void Update(float dt) override;

		void moveForward(float dt);
		void moveBackward(float dt);
		void moveLeft(float dt);
		void moveRight(float dt);
		void Rotate(float mouseX, float mouseY) override;
		
		void jump();

		void primaryAction();

		void secondaryAction();

		void setCamera(Camera* cam) {
			camera = cam;
		}

		void setJumpImpulse(float j) {
			jumpImpulse = j;
		}

		void setJumpCollider(Collider* collider) {
			jumpCollider = collider;
		}

		void SetScene(Scene* scene) {
			this->scene = scene;
		}

	private:
		Scene* scene = nullptr;
		bool isGrounded = false;
		float jumpCoolDown = 0.0f;
		Collider* jumpCollider = nullptr;
		float jumpImpulse = 1000.0f;
		Camera* camera = nullptr;
		float tSinceLastInput = 0.0f;
	};
}

