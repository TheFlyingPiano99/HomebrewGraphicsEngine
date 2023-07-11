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

}