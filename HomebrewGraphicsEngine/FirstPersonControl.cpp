#include "FirstPersonControl.h"
#include <iostream>


void hograengine::FirstPersonControl::update(float dt)
{
	if (camera == nullptr) {
		return;
	}
	ahead = camera->getLookDir();
	up = camera->getPreferedUp();
	right = glm::cross(ahead, up);
	ahead = glm::cross(up, right);
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
