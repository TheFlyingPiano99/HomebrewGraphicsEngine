#pragma once
#include "UserControl.h"
#include "Collider.h"
#include "Scene.h"
#include "SceneAudioSource.h"

namespace Hogra {
	class FirstPersonControl : public UserControl
	{
	public:

		FirstPersonControl() {
			allowRotate = true;
			GlobalVariables::hideCursor = true;
		}

		~FirstPersonControl() override;

		void BeforePhysicsLoopUpdate() override;

		void EarlyPhysicsUpdate(float dt) override;

		void LatePhysicsUpdate(float dt) override;

		void AfterPhysicsLoopUpdate() override;

		void MoveForward();
		void MoveBackward();
		void MoveLeft();
		void MoveRight();
		void Rotate(const glm::vec2& delta) override;
		
		void Jump();

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

		void SetLaserObject(SceneObject* object) {
			laser = object;
		}

		void SetLaserInpactLight(PointLight* light) {
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
		Camera* camera = nullptr;

		// Laser gun:
		SceneObject* laser = nullptr;
		PointLight* laserInpactLight = nullptr;
		SceneAudioSource* laserAudioSource = nullptr;
		SceneAudioSource* laserCoolDownAudioSource = nullptr;
		SceneAudioSource* laserChargeupAudioSource = nullptr;
		bool wasFiringLaser = false;
		bool isFiringLaser = false;

		//Jump and walk:
		SceneAudioSource* jumpAudioSource = nullptr;
		SceneAudioSource* footstepsAudioSource = nullptr;
		bool isGrounded = false;
		float jumpCoolDown = 0.0f;
		Collider* jumpCollider = nullptr;
		float jumpImpulse = 250.0f;
		bool isWalking = false;
		glm::vec3 walkForce;

		//Camera rotation:
		glm::vec2 headRotationDelta;
		float rotationSpeed = 0.25f;
		float rotationSlowDown = 0.4f;

		float tSinceLastInput = 0.0f;
	};
}

