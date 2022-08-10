#include "ControlAction.h"
#include "Callbacks.h"
#include "GUI.h"
#include <iostream>
#include "FirstPersonControl.h"

namespace Hogra {

	void MoveAvatarForward::execute(Scene* scene, float dt)
	{
		auto* control = scene->getAvatarControl();
		if (nullptr != control) {
			control->moveForward(dt);
		}
	}

	void MoveAvatarBackward::execute(Scene* scene, float dt)
	{
		auto* control = scene->getAvatarControl();
		if (nullptr != control) {
			control->moveBackward(dt);
		}
	}

	void MoveAvatarRight::execute(Scene* scene, float dt)
	{
		auto* control = scene->getAvatarControl();
		if (nullptr != control) {
			control->moveRight(dt);
		}
	}

	void MoveAvatarLeft::execute(Scene* scene, float dt)
	{
		auto* control = scene->getAvatarControl();
		if (nullptr != control) {
			control->moveLeft(dt);
		}
	}

	void MoveAvatarUp::execute(Scene* scene, float dt)
	{
		auto* control = scene->getAvatarControl();
		if (nullptr != control) {
			control->moveUp(dt);
		}
	}

	void MoveAvatarDown::execute(Scene* scene, float dt)
	{
		auto* control = scene->getAvatarControl();
		if (nullptr != control) {
			control->moveDown(dt);
		}
	}

	void ToggleGUI::execute(Scene* scene, float dt)
	{
		GUI::getInstance()->setVisible(!(GUI::getInstance()->isVisible()));
	}

	void FastForward::execute(Scene* scene, float dt)
	{
		scene->Update(dt * 100.0f);
	}

	void Rewind::execute(Scene* scene, float dt)
	{
		scene->Update(dt * -100.0f);
	}

	void TogglePause::execute(Scene* scene, float dt)
	{
		scene->TogglePause();
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
	void JumpAvatar::execute(Scene* scene, float dt)
	{
		auto* control = (FirstPersonControl*)scene->getAvatarControl();
		if (nullptr != control) {
 			control->jump();
		}
	}
	void ToggleDebugInfo::execute(Scene* scene, float dt)
	{
		scene->setDrawDebug(!scene->getDrawDebug());
	}
	void PrimaryAction::execute(Scene* scene, float dt)
	{
		auto* control = (FirstPersonControl*)scene->getAvatarControl();
		if (nullptr != control) {
			control->primaryAction(dt);
		}
	}

	void RestartAction::execute(Scene* scene, float dt)
	{
		SceneChange change;
		change.changeType = SceneChange::ChangeType::restartScene;
		scene->SetSceneChange(change);
	}

	void QuitAction::execute(Scene* scene, float dt)
	{
		SceneChange change;
		change.changeType = SceneChange::ChangeType::quit;
		scene->SetSceneChange(change);
	}
}