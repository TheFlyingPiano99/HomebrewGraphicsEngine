#pragma once
#include "UserControl.h"
#include "Collider.h"
#include "Scene.h"
#include "SceneAudioSource.h"

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

		void FrameBeginningControl() override;

		void FrameEndingControl() override;

		void Control(float dt) override;

		void moveForward(float dt);
		void moveBackward(float dt);
		void moveLeft(float dt);
		void moveRight(float dt);
		void Rotate(float mouseX, float mouseY) override;
		
		void Jump();

		void primaryAction(float dt);

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

		void SetLaserObject(SceneObject* object) {
			laser = object;
		}

		void SetLaserInpactLight(Light* light) {
			laserInpactLight = light;
		}

		void SetJumpAudioSource(SceneAudioSource* source) {
			jumpAudioSource = source;
		}

		void SetFootstepAudioSource(SceneAudioSource* source) {
			footstepsAudioSource = source;
		}

		void SetLaserAudioSource(SceneAudioSource* source) {
			laserAudioSource = source;
		}

		void SetLaserCooldownAudioSource(SceneAudioSource* source) {
			laserCoolDownAudioSource = source;
		}

		void SetLaserChargeupAudioSource(SceneAudioSource* source) {
			laserChargeupAudioSource = source;
		}

		bool PokeObject(const glm::vec2& ndcCoords, glm::vec3& pokePoint);

	private:
		Scene* scene = nullptr;
		SceneObject* laser = nullptr;
		Light* laserInpactLight = nullptr;
		SceneAudioSource* jumpAudioSource = nullptr;
		SceneAudioSource* footstepsAudioSource = nullptr;

		SceneAudioSource* laserAudioSource = nullptr;
		SceneAudioSource* laserCoolDownAudioSource = nullptr;
		SceneAudioSource* laserChargeupAudioSource = nullptr;
		bool isGrounded = false;
		float jumpCoolDown = 0.0f;
		Collider* jumpCollider = nullptr;
		float jumpImpulse = 1000.0f;
		Camera* camera = nullptr;
		float tSinceLastInput = 0.0f;
		bool isWalking = false;
		bool wasFiringLaser = false;
		bool isFiringLaser = false;
	};
}

