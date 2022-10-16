#include "ObservObjectControl.h"

Hogra::ObservObjectControl::ObservObjectControl() : rotationSpeed(0.01f), zoomSpeed(0.1f) {
	GlobalVariables::hideCursor = false;
	for (int i = 0; i < 6; i++) {
		colliders[i] = nullptr;
	}
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
	glm::vec3 intersectionPoint;
	glm::vec3 intersectionNormal;
	Ray ray;
	ray.SetPosition(camera->GetPosition() + dir * 1.0f);
	ray.setDirection(dir);

	glm::vec3 w_point;
	glm::vec3 w_normal;
	auto collider = scene->IntersectRay(ray, w_point, w_normal);
	std::cout << "Collider: " << collider << std::endl;
	if (nullptr == collider) {
		return;
	}
	bool isOneOf6 = false;
	for (int i = 0; i < 6; i++) {
		if (colliders[i] == collider) {
			isOneOf6 = true;
			break;
		}
	}
	if (!isOneOf6) {	// Other collider found
		return;
	}
}

void Hogra::ObservObjectControl::releasePlane(float x, float y) {
	if (nullptr == grabbedCollider) {
		return;
	}
	std::cout << "Release " << x << " " << y << std::endl;
}