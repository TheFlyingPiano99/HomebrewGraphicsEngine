#include "ControlAction.h"
#include "Callbacks.h"
#include "GUI.h"
#include <iostream>
#include "FirstPersonControl.h"
#include "ControlActionManager.h"

namespace Hogra {

	void MoveAvatarForward::Execute(Scene& scene)
	{
		auto* control = scene.getAvatarControl();
		if (nullptr != control) {
			control->MoveForward();
		}
	}

	void MoveAvatarBackward::Execute(Scene& scene)
	{
		auto* control = scene.getAvatarControl();
		if (nullptr != control) {
			control->MoveBackward();
		}
	}

	void MoveAvatarRight::Execute(Scene& scene)
	{
		auto* control = scene.getAvatarControl();
		if (nullptr != control) {
			control->MoveRight();
		}
	}

	void MoveAvatarLeft::Execute(Scene& scene)
	{
		auto* control = scene.getAvatarControl();
		if (nullptr != control) {
			control->MoveLeft();
		}
	}

	void MoveAvatarUp::Execute(Scene& scene)
	{
		auto* control = scene.getAvatarControl();
		if (nullptr != control) {
			control->MoveUp();
		}
	}

	void MoveAvatarDown::Execute(Scene& scene)
	{
		auto* control = scene.getAvatarControl();
		if (nullptr != control) {
			control->MoveDown();
		}
	}

	void ToggleGUI::Execute(Scene& scene)
	{
		GUI::getInstance()->setVisible(!(GUI::getInstance()->isVisible()));
	}

	void TogglePause::Execute(Scene& scene)
	{
		scene.TogglePause();
	}

	void ToggleFullScreenMode::Execute(Scene& scene)
	{
		Callbacks::toggleFullScreen();
	}

	int ButtonKeyAction::getKey() const
	{
		return key;
	}

	void ButtonKeyAction::OnPress(int _key, int _scancode, int _mods)
	{
		pressed = true;
	}

	void ButtonKeyAction::OnRelease(int _key, int _scancode, int _mods)
	{
		pressed = false;
	}

	bool ButtonKeyAction::PopIsTriggering()
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
	void JumpAvatar::Execute(Scene& scene)
	{
		auto* control = (FirstPersonControl*)scene.getAvatarControl();
		if (nullptr != control) {
 			control->Jump();
		}
	}
	void ToggleDebugInfo::Execute(Scene& scene)
	{
		scene.setDrawDebug(!scene.getDrawDebug());
	}
	void PrimaryAction::Execute(Scene& scene)
	{
		auto* control = (FirstPersonControl*)scene.getAvatarControl();
		if (nullptr != control) {
			control->primaryAction();
		}
	}

	void RestartAction::Execute(Scene& scene)
	{
		SceneChange change;
		change.changeType = SceneChange::ChangeType::restartScene;
		scene.SetSceneChange(change);
	}

	void QuitAction::Execute(Scene& scene)
	{
		SceneChange change;
		change.changeType = SceneChange::ChangeType::quit;
		scene.SetSceneChange(change);
	}

	bool AxisMoveAction::PopIsTriggering()
	{
		auto temp = movedInThisFrame;
		movedInThisFrame = false;
		return temp;
	}

	void AxisMoveAction::OnMove(const glm::vec2& _pixPos)
	{
		this->pixDelta = _pixPos - this->pixPos;
		this->pixPos = _pixPos;
		movedInThisFrame = true;
	}
	void CameraMoveAction::Execute(Scene& scene)
	{
		scene.getAvatarControl()->Rotate(-(pixPos - glm::vec2(GlobalVariables::windowWidth, GlobalVariables::windowHeight) * 0.5f)* 0.01f);
	}
	void SecondaryAction::Execute(Scene& scene)
	{
		// Todo
	}
}