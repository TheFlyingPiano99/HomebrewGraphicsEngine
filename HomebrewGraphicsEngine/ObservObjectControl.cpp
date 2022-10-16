#include "ObservObjectControl.h"

Hogra::ObservObjectControl::ObservObjectControl() : rotationSpeed(0.01f), zoomSpeed(0.1f) {
	GlobalVariables::hideCursor = false;
}

void Hogra::ObservObjectControl::Rotate(const glm::vec2& delta)
{
	if (nullptr == camera || !GlobalVariables::hideCursor) {
		return;
	}
	camera->RotateAroundPoint(delta * rotationSpeed);
}

void Hogra::ObservObjectControl::Zoom(float delta)
{
	if (nullptr == camera) {
		return;
	}
	if (isPlaneGrabbed) {
		DragPlane(delta * 0.1f);
		return;
	}
	camera->ApproachCenter(delta * zoomSpeed);
}

void Hogra::ObservObjectControl::primaryAction()
{

}

void Hogra::ObservObjectControl::grab()
{
	GlobalVariables::hideCursor = true;
}

void Hogra::ObservObjectControl::release()
{
	GlobalVariables::hideCursor = false;
}

void Hogra::ObservObjectControl::grabPlane(float x, float y) {
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
		isPlaneGrabbed = true;
		planeNormal = w_normal;
		std::cout << "N: " << planeNormal.x << ", " << planeNormal.y << ", " << planeNormal.z << std::endl;
	}
}

void Hogra::ObservObjectControl::releasePlane(float x, float y) {
	isPlaneGrabbed = false;

}

void Hogra::ObservObjectControl::DragPlane(float delta) {
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
	collider->SetMin(min);
	collider->SetMax(max);
	volumeObject->ResizeDisplayBoundingBox(min, max);
}
