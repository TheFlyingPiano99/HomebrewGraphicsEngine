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
	camera->ApproachCenter(delta * zoomSpeed);
}

void Hogra::ObservObjectControl::primaryAction()
{
	if (!GlobalVariables::hideCursor) {
		volumeObject->CycleSelectedFeature();
	}
}

void Hogra::ObservObjectControl::grab()
{
	GlobalVariables::hideCursor = true;
}

void Hogra::ObservObjectControl::release()
{
	GlobalVariables::hideCursor = false;
}
