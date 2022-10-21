#include "ControlAction.h"
#include "Callbacks.h"
#include "GUI.h"
#include <iostream>
#include "FirstPersonControl.h"
#include "ControlActionManager.h"
#include "ObserveObjectControl.h"

namespace Hogra {

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

	bool AxisMoveAction::PopIsTriggering()
	{
		auto temp = movedInThisFrame;
		movedInThisFrame = false;
		return temp;
	}

	void AxisMoveAction::OnAxisMove(const glm::vec2& _pixPos, bool isFirst)
	{
		if (isFirst) {
			this->pixPos = glm::vec2(GlobalVariables::windowWidth, GlobalVariables::windowHeight) / 2.0f;
		}
		this->pixDelta = _pixPos - this->pixPos;
		this->pixPos = _pixPos;
		movedInThisFrame = true;
	}

	/*
		void MoveAvatarForward::Execute(Scene& scene)
	{
		auto* control = scene.GetUserControl();
		if (nullptr != control) {
			control->MoveForward();
		}
	}

	void MoveAvatarBackward::Execute(Scene& scene)
	{
		auto* control = scene.GetUserControl();
		if (nullptr != control) {
			control->MoveBackward();
		}
	}

	void MoveAvatarRight::Execute(Scene& scene)
	{
		auto* control = scene.GetUserControl();
		if (nullptr != control) {
			control->MoveRight();
		}
	}

	void MoveAvatarLeft::Execute(Scene& scene)
	{
		auto* control = scene.GetUserControl();
		if (nullptr != control) {
			control->MoveLeft();
		}
	}

	void MoveAvatarUp::Execute(Scene& scene)
	{
		auto* control = scene.GetUserControl();
		if (nullptr != control) {
			control->MoveUp();
		}
	}

	void MoveAvatarDown::Execute(Scene& scene)
	{
		auto* control = scene.GetUserControl();
		if (nullptr != control) {
			control->MoveDown();
		}
	}

	void ToggleGUI::Execute(Scene& scene)
	{
		GUI::getInstance()->setVisible(!(GUI::getInstance()->IsVisible()));
	}

	void ToggleHUD::Execute(Scene& scene)
	{
	}

	void StepFeature::Execute(Scene& scene)
	{
	}

	void TogglePause::Execute(Scene& scene)
	{
		scene.TogglePause();
	}

	void ToggleFullScreenMode::Execute(Scene& scene)
	{
		Callbacks::toggleFullScreen();
	}
	void JumpAvatar::Execute(Scene& scene)
	{
		auto* control = (FirstPersonControl*)scene.GetUserControl();
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
		auto* control = (FirstPersonControl*)scene.GetUserControl();
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

	void CameraMoveAction::Execute(Scene& scene)
	{
		scene.GetUserControl()->Rotate(-pixDelta);
	}
	void SecondaryAction::Execute(Scene& scene)
	{
		// Todo
	}

	void ClickOnScreen::Execute(Scene& scene) {
		if (GlobalVariables::hideCursor) {
			return;
		}
		double x;
		double y;
		glfwGetCursorPos(GlobalVariables::window, &x, &y);
		float ndc_x = x / (double)GlobalVariables::windowWidth * 2.0 - 1.0;
		float ndc_y = y / (double)GlobalVariables::windowHeight * 2.0 - 1.0;
		/*
		bool isSuccess = scene.GetVolumeObjects()[0]->SelectTransferFunctionRegion(ndc_x, ndc_y);
		if (!isSuccess) {
			((ObservObjectControl*)scene.GetUserControl())->grabPlane(ndc_x, ndc_y);
		}
	}
	*/
	/*
	void ReleaseClickOnScreen::Execute(Scene& scene) {
		double x;
		double y;
		glfwGetCursorPos(GlobalVariables::window, &x, &y);
		float ndc_x = x / (double)GlobalVariables::windowWidth * 2.0 - 1.0;
		float ndc_y = y / (double)GlobalVariables::windowHeight * 2.0 - 1.0;
		((ObservObjectControl*)scene.GetUserControl())->releasePlane(ndc_x, ndc_y);
	}


	void CameraZoomAction::Execute(Scene& scene)
	{
		scene.GetUserControl()->Zoom(this->pixPos.y);
	}
	void GrabAction::Execute(Scene& scene)
	{
		scene.GetUserControl()->grab();
	}	void selectTransferFunctionRegion(double xpos, double ypos);

	void ReleaseAction::Execute(Scene& scene)
	{
		scene.GetUserControl()->release();
	}*/
}