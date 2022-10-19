#include "FirstPersonControl.h"
#include <iostream>
#include "CameraAnimation.h"
#include "MemoryManager.h"
#include "SceneEventManager.h"
#include "SceneEventImplementation.h"
#include "HograTime.h"

void Hogra::FirstPersonControl::MoveForward()
{
	if (physics == nullptr || !allowMove) {
		footstepsAudioSource->Stop();
		return;
	}
	walkForce += ahead * propellingForce;
	if (camera->getAnimation() == nullptr) {
		auto* a = Allocator::New<HeadBob>();
		camera->setAnimation(a);
	}
	if (isGrounded) {
		if (!footstepsAudioSource->IsPlaying()) {
			footstepsAudioSource->Play();
		}
		isWalking = true;
	}
	tSinceLastInput = 0.0f;
}

void Hogra::FirstPersonControl::MoveBackward()
{
	if (physics == nullptr || !allowMove) {
		footstepsAudioSource->Stop();
		return;
	}
	walkForce += -ahead * propellingForce;
	if (camera->getAnimation() == nullptr) {
		auto* a = Allocator::New<HeadBob>();
		camera->setAnimation(a);
	}
	if (isGrounded) {
		if (!footstepsAudioSource->IsPlaying()) {
			footstepsAudioSource->Play();
		}
		isWalking = true;
	}
	tSinceLastInput = 0.0f;
}

void Hogra::FirstPersonControl::MoveLeft()
{
	if (physics == nullptr || !allowMove) {
		footstepsAudioSource->Stop();
		return;
	}
	walkForce += -right * propellingForce;
	if (camera->getAnimation() == nullptr) {
		auto* a = Allocator::New<HeadBob>();
		camera->setAnimation(a);
	}
	if (isGrounded) {
		if (!footstepsAudioSource->IsPlaying()) {
			footstepsAudioSource->Play();
		}
		isWalking = true;
	}
	tSinceLastInput = 0.0f;
}

void Hogra::FirstPersonControl::MoveRight()
{
	if (physics == nullptr || !allowMove) {
		footstepsAudioSource->Stop();
		return;
	}
	walkForce += right * propellingForce;
	if (camera->getAnimation() == nullptr) {
		auto* a = Allocator::New<HeadBob>();
		camera->setAnimation(a);
	}
	if (isGrounded) {
		if (!footstepsAudioSource->IsPlaying()) {
			footstepsAudioSource->Play();
		}
		isWalking = true;
	}
	tSinceLastInput = 0.0f;
}

Hogra::FirstPersonControl::~FirstPersonControl()
{
}

void Hogra::FirstPersonControl::EarlyPhysicsUpdate(float dt)
{
	if (jumpCollider != nullptr) {
		isGrounded = jumpCollider->PopCollided();
	}
	if (jumpCoolDown > 0.0f) {
		jumpCoolDown -= dt;
		if (jumpCoolDown < 0.0f) {
			jumpCoolDown = 0.0f;
		}
	}
	if (camera == nullptr) {
		return;
	}
	
	camera->Rotate(headRotationDelta * dt);
	if (tSinceLastInput > 0.5f) {
		auto* a = camera->getAnimation();
		HeadBob* pointer = (HeadBob*) & a;
		Allocator::Delete(pointer);
		camera->setAnimation(nullptr);
	}
	else {
		tSinceLastInput += dt;
	}
	if (physics != nullptr) {
		physics->applyTransientForce(walkForce);
	}
}

void Hogra::FirstPersonControl::LatePhysicsUpdate(float dt)
{
	if (camera == nullptr) {
		return;
	}
	ahead = camera->GetLookDir();
	up = camera->getPreferedUp();
	right = glm::cross(ahead, up);
	ahead = glm::cross(up, right);
}

void Hogra::FirstPersonControl::BeforePhysicsLoopUpdate()
{
	if (nullptr != laser) {
		laser->SetIsVisible(false);
		laserInpactLight->SetIsActive(false);
	}
	isWalking = false;
	isFiringLaser = false;
	if (wasFiringLaser) {
		laserCoolDownAudioSource->Play();
	}
	walkForce = glm::vec3(0.0f);
	headRotationDelta *= rotationSlowDown;
}

void Hogra::FirstPersonControl::AfterPhysicsLoopUpdate()
{
	if (!isWalking) {
		footstepsAudioSource->Stop();
	}
	if (!isFiringLaser) {
		laserAudioSource->Stop();
		laserChargeupAudioSource->Stop();
	}
	wasFiringLaser = isFiringLaser;
}

void Hogra::FirstPersonControl::Rotate(const glm::vec2& delta)
{
	if (camera == nullptr || !allowRotate) {
		return;
	}
	headRotationDelta += delta * rotationSpeed;
}

void Hogra::FirstPersonControl::Jump() {
	if (physics == nullptr || !allowMove || !isGrounded || jumpCoolDown > 0.0f) {
		return;
	}
	physics->ApplyImpulse(up * jumpImpulse, glm::vec3(0.0f));
	jumpCoolDown = 1.0f;
	jumpAudioSource->Play();
	footstepsAudioSource->Stop();
}

void Hogra::FirstPersonControl::primaryAction()
{
	glm::vec3 point;
	bool isPoke = false;
	if (nullptr != scene) {
		isPoke = PokeObject(glm::vec2(0.0f, 0.0f), point);
	}

	if (nullptr != laser) {
		laser->SetIsVisible(true);
		glm::vec3 start = camera->GetPosition() - glm::vec3(0.0f, 0.5f, 0.0f);
		glm::vec3 end;
		if (isPoke) {
			end = point;
		}
		else {
			end = start + camera->GetLookDir() * 1000.0f;
		}
		glm::vec3 lookAt = (start + end) / 2.0f;
		laser->SetPosition(lookAt);
		float beamLength = glm::length(start - end);
		laser->SetScale(glm::vec3(0.05f, 0.5f * beamLength, 0.05f));
		auto dir = glm::normalize(end - start);
		laser->SetOrientation(glm::rotation(glm::vec3(0.0f, 1.0f, 0.0f), dir));
		laserInpactLight->SetIsActive(true);
		laserInpactLight->SetPosition(end - dir * 0.1f);
		if (!laserAudioSource->IsPlaying()) {
			laserAudioSource->Play();
		}
		if (!wasFiringLaser) {
			laserChargeupAudioSource->Play();
		}
		laserCoolDownAudioSource->Stop();
		isFiringLaser = true;
	}
}

void Hogra::FirstPersonControl::secondaryAction()
{
	//TODO
}

bool Hogra::FirstPersonControl::PokeObject(const glm::vec2& ndcCoords, glm::vec3& pokePoint)
{
	glm::vec4 wDir = camera->GetRayDirMatrix() * glm::vec4(ndcCoords, 0.0, 1.0f);
	wDir /= wDir.w;
	glm::vec3 dir = glm::normalize(glm::vec3(wDir));
	glm::vec3 intersectionPoint;
	glm::vec3 intersectionNormal;
	Ray ray;
	ray.SetPosition(camera->GetPosition() + dir * 1.0f);
	ray.setDirection(dir);
	const auto* selected = scene->IntersectRay(ray, intersectionPoint, intersectionNormal);
	if (nullptr != selected && nullptr != selected->GetPhysics()) {
		selected->GetPhysics()->ApplyImpulse(dir * 10.0f, intersectionPoint - selected->GetPhysics()->getOwnerPosition());
		pokePoint = intersectionPoint;
		return true;
	}
	return false;
}
