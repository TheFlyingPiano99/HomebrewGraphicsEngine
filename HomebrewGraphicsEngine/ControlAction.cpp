#include "ControlAction.h"
#include "Callbacks.h"
#include "GUI.h"

bool ControlAction::isThisAction(int key, int scancode, int action)
{
	return (this->key == key
		&& (
			(enableRepeat && (action == GLFW_PRESS || action == GLFW_REPEAT))
		|| (!enableRepeat && action == GLFW_PRESS)
			));
}

//---------------------------------------------------------------------------

void MoveCameraForward::execute(Scene* scene, float dt)
{
	Camera* camera = scene->getCamera();
	if (nullptr != camera) {
		camera->moveForward(dt);
	}
}

void MoveCameraBackward::execute(Scene* scene, float dt)
{
	Camera* camera = scene->getCamera();
	if (nullptr != camera) {
		camera->moveBackward(dt);
	}
}

void ToggleGUI::execute(Scene* scene, float dt)
{
	GUI::getInstance()->setVisible(!(GUI::getInstance()->isVisible()));
}

void FastForward::execute(Scene* scene, float dt)
{
	scene->animate(dt * 100.0f);
}

void Rewind::execute(Scene* scene, float dt)
{
	scene->animate(dt * -100.0f);
}

void TogglePause::execute(Scene* scene, float dt)
{
	scene->togglePause();
}

void ToggleFullScreenMode::execute(Scene* scene, float dt)
{
	Callbacks::toggleFullScreen();
}
