#include "UserControl.h"

void Hogra::UserControl::MoveForward()
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(ahead * (orientationProvider->GetOrientation() * propellingForce));
}

void Hogra::UserControl::MoveBackward()
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(-ahead * (orientationProvider->GetOrientation() * propellingForce));
}

void Hogra::UserControl::MoveLeft()
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(-right * (orientationProvider->GetOrientation() * propellingForce));
}

void Hogra::UserControl::MoveRight()
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(right * (orientationProvider->GetOrientation() * propellingForce));
}

void Hogra::UserControl::MoveUp()
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(up * (orientationProvider->GetOrientation() * propellingForce));
}

void Hogra::UserControl::MoveDown()
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(-up * (orientationProvider->GetOrientation() * propellingForce));
}

void Hogra::UserControl::Rotate(const glm::vec2& ndcDelta)
{
	if (physics == nullptr || !allowRotate) {
		return;
	}
	physics->applyTransientTorque(rotatingTorque * glm::vec3(ndcDelta.y, ndcDelta.x, 0.0f));
}

void Hogra::UserControl::Zoom(float delta)
{
}

void Hogra::UserControl::primaryAction()
{
}

void Hogra::UserControl::secondaryAction()
{
}

void Hogra::UserControl::grab()
{
}

void Hogra::UserControl::release()
{
}

void Hogra::UserControl::EarlyPhysicsUpdate(float dt)
{

}

void Hogra::UserControl::LatePhysicsUpdate(float dt)
{
	if (nullptr == physics) {
		return;
	}
	ahead = orientationProvider->GetOrientation() * initialDirection;
	right = glm::cross(ahead, initialUp);
	up = glm::cross(right, ahead);
}
