#include "UserControl.h"

std::vector<Hogra::UserControl*> Hogra::UserControl::heapAllocatedInstances = std::vector<Hogra::UserControl*>();

void Hogra::UserControl::moveForward(float dt)
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(ahead * (orientationProvider->GetOrientation() * propellingForce));
}

void Hogra::UserControl::moveBackward(float dt)
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(-ahead * (orientationProvider->GetOrientation() * propellingForce));
}

void Hogra::UserControl::moveLeft(float dt)
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(-right * (orientationProvider->GetOrientation() * propellingForce));
}

void Hogra::UserControl::moveRight(float dt)
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(right * (orientationProvider->GetOrientation() * propellingForce));
}

void Hogra::UserControl::moveUp(float dt)
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(up * (orientationProvider->GetOrientation() * propellingForce));
}

void Hogra::UserControl::moveDown(float dt)
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(-up * (orientationProvider->GetOrientation() * propellingForce));
}

void Hogra::UserControl::Rotate(float mouseX, float mouseY)
{
	if (physics == nullptr || !allowRotate) {
		return;
	}
	physics->applyTransientTorque(rotatingTorque * glm::vec3(mouseY, mouseX, 0.0f));
}

void Hogra::UserControl::Control(float dt)
{

}

void Hogra::UserControl::Update(float dt)
{
	if (nullptr == physics) {
		return;
	}
	ahead = orientationProvider->GetOrientation() * initialDirection;
	right = glm::cross(ahead, initialUp);
	up = glm::cross(right, ahead);
}
