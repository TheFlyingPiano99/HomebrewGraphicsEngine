#include "FirstPersonControl.h"
#include <iostream>
#include "CameraAnimation.h"

void hograengine::FirstPersonControl::moveForward(float dt)
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(ahead * (physics->getOwnerOrientation() * propellingForce));
	if (camera->getAnimation() == nullptr) {
		auto* a = new HeadBob();
		camera->setAnimation(a);
	}
	tSinceLastInput = 0.0f;
}

void hograengine::FirstPersonControl::moveBackward(float dt)
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(-ahead * (physics->getOwnerOrientation() * propellingForce));
	if (camera->getAnimation() == nullptr) {
		auto* a = new HeadBob();
		camera->setAnimation(a);
	}
	tSinceLastInput = 0.0f;
}

void hograengine::FirstPersonControl::moveLeft(float dt)
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(-right * (physics->getOwnerOrientation() * propellingForce));
	if (camera->getAnimation() == nullptr) {
		auto* a = new HeadBob();
		camera->setAnimation(a);
	}
	tSinceLastInput = 0.0f;
}

void hograengine::FirstPersonControl::moveRight(float dt)
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(right * (physics->getOwnerOrientation() * propellingForce));
	if (camera->getAnimation() == nullptr) {
		auto* a = new HeadBob();
		camera->setAnimation(a);
	}
	tSinceLastInput = 0.0f;
}

void hograengine::FirstPersonControl::update(float dt)
{
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

void hograengine::FirstPersonControl::rotate(float mouseX, float mouseY)
{
	if (camera == nullptr || !allowRotate) {
		return;
	}
	camera->rotate(mouseX, mouseY);
}

void hograengine::FirstPersonControl::jump() {
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyImpulse(up * jumpImpulse, glm::vec3(0.0f));
}
