#include "ControlAction.h"
#include "Callbacks.h"
#include "GUI.h"
#include <iostream>

namespace hograengine {

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

	void MoveCameraRight::execute(Scene* scene, float dt)
	{
		Camera* camera = scene->getCamera();
		if (nullptr != camera) {
			camera->moveRight(dt);
		}
	}

	void MoveCameraLeft::execute(Scene* scene, float dt)
	{
		Camera* camera = scene->getCamera();
		if (nullptr != camera) {
			camera->moveLeft(dt);
		}
	}

	void MoveCameraUp::execute(Scene* scene, float dt)
	{
		Camera* camera = scene->getCamera();
		if (nullptr != camera) {
			camera->moveUp(dt);
		}
	}

	void MoveCameraDown::execute(Scene* scene, float dt)
	{
		Camera* camera = scene->getCamera();
		if (nullptr != camera) {
			camera->moveDown(dt);
		}
	}

	void ToggleGUI::execute(Scene* scene, float dt)
	{
		GUI::getInstance()->setVisible(!(GUI::getInstance()->isVisible()));
	}

	void FastForward::execute(Scene* scene, float dt)
	{
		scene->update(dt * 100.0f);
	}

	void Rewind::execute(Scene* scene, float dt)
	{
		scene->update(dt * -100.0f);
	}

	void TogglePause::execute(Scene* scene, float dt)
	{
		scene->togglePause();
	}

	void ToggleFullScreenMode::execute(Scene* scene, float dt)
	{
		Callbacks::toggleFullScreen();
	}

	int ControlAction::getKey() const
	{
		return key;
	}

	void ControlAction::onPress(int _key, int _scancode, int _mods)
	{
		pressed = true;
	}

	void ControlAction::onRelease(int _key, int _scancode, int _mods)
	{
		pressed = false;
	}

	bool ControlAction::isTriggering()
	{
		bool trigger;
		switch (triggerType)
		{
		case TriggerType::triggerOnPress:
			trigger = pressed && !pressedPreviously;
			break;
		case TriggerType::triggerContinuosly:
			trigger = pressed;
			break;
		case TriggerType::triggerOnRelease:
			trigger = !pressed && pressedPreviously;
			break;
		default:
			trigger = false;
			break;
		}
		pressedPreviously = pressed;
		return trigger;
	}
}