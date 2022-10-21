#include "ObserveObjectControl.h"

Hogra::ObserveObjectControl::ObserveObjectControl() : rotationSpeed(0.01f), zoomSpeed(0.1f) {
	GlobalVariables::hideCursor = false;
}

void Hogra::ObserveObjectControl::Rotate(const glm::vec2& delta)
{
	if (nullptr == camera || !GlobalVariables::hideCursor) {
		return;
	}
	if (isPlaneGrabbed) {
		std::cout << delta.x << ", " << delta.y << std::endl;
		glm::vec3 w_d = (-camera->getRight() * delta.x + camera->getUp() * delta.y) / length(planePosition - camera->GetPosition());
		float d = glm::dot(planeNormal, w_d);
		std::cout << "Delta: " << w_d.x << ", " << w_d.y << ", " << w_d.z << std::endl;
		std::cout << "D: " << d << std::endl;
		DragPlane(d);
		return;
	}
	camera->RotateAroundPoint(delta * rotationSpeed);
}

void Hogra::ObserveObjectControl::Zoom(float delta)
{
	if (nullptr == camera) {
		return;
	}

	camera->ApproachCenter(delta * zoomSpeed);
}

void Hogra::ObserveObjectControl::primaryAction()
{

}

void Hogra::ObserveObjectControl::grab()
{
	GlobalVariables::hideCursor = true;
}

void Hogra::ObserveObjectControl::release()
{
	GlobalVariables::hideCursor = false;
}

void Hogra::ObserveObjectControl::grabPlane(float x, float y) {

	glm::vec4 wDir = camera->GetRayDirMatrix() * glm::vec4(x, y, 0.0, 1.0f);
	wDir /= wDir.w;
	glm::vec3 dir = glm::normalize(glm::vec3(wDir));
	Ray ray;
	ray.SetPosition(camera->GetPosition());
	ray.setDirection(dir);

	glm::vec3 w_point(0.0f);
	glm::vec3 w_normal(0.0f);
	auto collidedWith = scene->IntersectRay(ray, w_point, w_normal);
	std::cout << "Collider: " << collidedWith << std::endl;
	if (nullptr == collidedWith) {
		return;
	}
	if (collider == collidedWith) {
		GlobalVariables::hideCursor = true;
		isPlaneGrabbed = true;
		planePosition = w_point;
		planeNormal = w_normal;
		std::cout << "N: " << planeNormal.x << ", " << planeNormal.y << ", " << planeNormal.z << std::endl;
	}
}

void Hogra::ObserveObjectControl::releasePlane(float x, float y) {
	isPlaneGrabbed = false;
	GlobalVariables::hideCursor = false;

}

void Hogra::ObserveObjectControl::DragPlane(float delta) {
	if (nullptr == camera) {
		return;
	}
	glm::vec3 min(0.0f);
	glm::vec3 max(0.0f);
	volumeObject->GetMinAndMax(min, max);
	if (glm::dot(planeNormal, glm::vec3(1,0,0)) > 0.9f) {
		max += glm::vec3(delta, 0, 0);
	}
	else if (glm::dot(planeNormal, glm::vec3(0, 1, 0)) > 0.9f) {
		max += glm::vec3(0, delta, 0);
	}
	else if (glm::dot(planeNormal, glm::vec3(0, 0, 1)) > 0.9f) {
		max += glm::vec3(0, 0, delta);
	}
	else if (glm::dot(planeNormal, glm::vec3(-1, 0, 0)) > 0.9f) {
		min += glm::vec3(-delta, 0, 0);
	}
	else if (glm::dot(planeNormal, glm::vec3(0, -1, 0)) > 0.9f) {
		min += glm::vec3(0, -delta, 0);
	}
	else if (glm::dot(planeNormal, glm::vec3(0, 0, -1)) > 0.9f) {
		min += glm::vec3(0, 0, -delta);
	}
	volumeObject->ResizeDisplayBoundingBox(min, max);
	volumeObject->GetMinAndMax(min, max);
	collider->SetMin(min);
	collider->SetMax(max);
}
