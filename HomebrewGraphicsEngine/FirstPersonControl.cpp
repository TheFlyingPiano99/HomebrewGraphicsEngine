#include "FirstPersonControl.h"
#include <iostream>
#include "CameraAnimation.h"
#include "MemoryManager.h"

void Hogra::FirstPersonControl::moveForward(float dt)
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(ahead * (orientationProvider->GetOrientation() * propellingForce));
	if (camera->getAnimation() == nullptr) {
		auto* a = new HeadBob();
		camera->setAnimation(a);
	}
	tSinceLastInput = 0.0f;
}

void Hogra::FirstPersonControl::moveBackward(float dt)
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(-ahead * (orientationProvider->GetOrientation() * propellingForce));
	if (camera->getAnimation() == nullptr) {
		auto* a = new HeadBob();
		camera->setAnimation(a);
	}
	tSinceLastInput = 0.0f;
}

void Hogra::FirstPersonControl::moveLeft(float dt)
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(-right * (orientationProvider->GetOrientation() * propellingForce));
	if (camera->getAnimation() == nullptr) {
		auto* a = new HeadBob();
		camera->setAnimation(a);
	}
	tSinceLastInput = 0.0f;
}

void Hogra::FirstPersonControl::moveRight(float dt)
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(right * (orientationProvider->GetOrientation() * propellingForce));
	if (camera->getAnimation() == nullptr) {
		auto* a = new HeadBob();
		camera->setAnimation(a);
	}
	tSinceLastInput = 0.0f;
}

Hogra::FirstPersonControl* Hogra::FirstPersonControl::Instantiate()
{
	auto* instance = new FirstPersonControl();
	MemoryManager::heapAllocatedInstances.push_back(instance);
	return instance;
}

Hogra::FirstPersonControl::~FirstPersonControl()
{
}

void Hogra::FirstPersonControl::Update(float dt)
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
	ahead = camera->getLookDir();
	up = camera->getPreferedUp();
	right = glm::cross(ahead, up);
	ahead = glm::cross(up, right);
	if (tSinceLastInput > 0.5f) {
		auto* a = camera->getAnimation();
		delete a;
		camera->setAnimation(nullptr);
	}
	else {
		tSinceLastInput += dt;
	}
}

void Hogra::FirstPersonControl::Rotate(float mouseX, float mouseY)
{
	if (camera == nullptr || !allowRotate) {
		return;
	}
	camera->Rotate(mouseX, mouseY);
}

void Hogra::FirstPersonControl::jump() {
	if (physics == nullptr || !allowMove || !isGrounded || jumpCoolDown > 0.0f) {
		return;
	}
	physics->applyImpulse(up * jumpImpulse, glm::vec3(0.0f));
	jumpCoolDown = 1.0f;
}
