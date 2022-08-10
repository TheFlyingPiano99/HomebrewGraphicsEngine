#include "FirstPersonControl.h"
#include <iostream>
#include "CameraAnimation.h"
#include "MemoryManager.h"
#include "SceneEventManager.h"
#include "SceneEventImplementation.h"

void Hogra::FirstPersonControl::moveForward(float dt)
{
	if (physics == nullptr || !allowMove) {
		return;
	}
	physics->applyTransientForce(ahead * propellingForce);
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
	physics->applyTransientForce(-ahead * propellingForce);
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
	physics->applyTransientForce(-right * propellingForce);
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
	physics->applyTransientForce(right * propellingForce);
	if (camera->getAnimation() == nullptr) {
		auto* a = new HeadBob();
		camera->setAnimation(a);
	}
	tSinceLastInput = 0.0f;
}

Hogra::FirstPersonControl* Hogra::FirstPersonControl::Instantiate()
{
	auto* instance = new FirstPersonControl();
	heapAllocatedInstances.push_back((UserControl*)instance);
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

void Hogra::FirstPersonControl::PreUserInputControl(float dt)
{
	if (nullptr != laser) {
		laser->SetIsVisible(false);
		laserInpactLight->SetIsActive(false);
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
	physics->ApplyImpulse(up * jumpImpulse, glm::vec3(0.0f));
	jumpCoolDown = 1.0f;
}

void Hogra::FirstPersonControl::primaryAction(float dt)
{
	glm::vec3 point;
	bool isPoke = false;
	if (nullptr != scene) {
		isPoke = PokeObject(glm::vec2(0.0f, 0.0f), point);
	}

	if (nullptr != laser) {
		laser->SetIsVisible(true);
		glm::vec3 start = camera->getEyePos() - glm::vec3(0.0f, 0.5f, 0.0f);
		glm::vec3 end;
		if (isPoke) {
			end = point;
		}
		else {
			end = start + camera->getLookDir() * 1000.0f;
		}
		glm::vec3 center = (start + end) / 2.0f;
		laser->SetPosition(center);
		float beamLength = glm::length(start - end);
		laser->SetScale(glm::vec3(0.05f, 0.5f * beamLength, 0.05f));
		auto dir = glm::normalize(end - start);
		laser->SetOrientation(glm::rotation(glm::vec3(0.0f, 1.0f, 0.0f), dir));
		laserInpactLight->SetIsActive(true);
		laserInpactLight->SetPosition(end - dir * 0.1f);
	}
}

void Hogra::FirstPersonControl::secondaryAction()
{
	//TODO
}

bool Hogra::FirstPersonControl::PokeObject(const glm::vec2& ndcCoords, glm::vec3& pokePoint)
{
	glm::vec4 wDir = camera->getRayDirMatrix() * glm::vec4(ndcCoords, 0.0, 1.0f);
	wDir /= wDir.w;
	glm::vec3 dir = glm::normalize(glm::vec3(wDir));
	glm::vec3 intersectionPoint;
	glm::vec3 intersectionNormal;
	Ray ray;
	ray.SetPosition(camera->getEyePos() + dir * 1.0f);
	ray.setDirection(dir);
	const auto* selected = scene->IntersectRay(ray, intersectionPoint, intersectionNormal);
	if (nullptr != selected && nullptr != selected->GetPhysics()) {
		selected->GetPhysics()->ApplyImpulse(dir * 10.0f, intersectionPoint - selected->GetPhysics()->getOwnerPosition());
		pokePoint = intersectionPoint;
		return true;
	}
	return false;
}
