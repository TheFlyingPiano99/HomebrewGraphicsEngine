#include "UserControl.h"

void hograengine::UserControl::moveForward(float dt)
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(ahead * (physics->getOwnerOrientation() * propellingForce));
}

void hograengine::UserControl::moveBackward(float dt)
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(-ahead * (physics->getOwnerOrientation() * propellingForce));
}

void hograengine::UserControl::moveLeft(float dt)
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(-right * (physics->getOwnerOrientation() * propellingForce));
}

void hograengine::UserControl::moveRight(float dt)
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(right * (physics->getOwnerOrientation() * propellingForce));
}

void hograengine::UserControl::moveUp(float dt)
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(up * (physics->getOwnerOrientation() * propellingForce));
}

void hograengine::UserControl::moveDown(float dt)
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(-up * (physics->getOwnerOrientation() * propellingForce));
}

void hograengine::UserControl::rotate(float mouseX, float mouseY)
{
	if (physics == nullptr || !allowRotate) {
		return;
	}
	physics->applyTransientTorque(rotatingTorque * glm::vec3(mouseY, mouseX, 0.0f));
}

void hograengine::UserControl::control(float dt)
{

}

void hograengine::UserControl::update(float dt)
{
	if (nullptr == physics) {
		return;
	}
	ahead = physics->getOwnerOrientation() * initialDirection;
	right = glm::cross(ahead, initialUp);
	up = glm::cross(right, ahead);
}
