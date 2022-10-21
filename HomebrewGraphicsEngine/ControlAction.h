#pragma once

#include "Scene.h"
#include <queue>
#include <GLFW/glfw3.h>

namespace Hogra {

	class AbstractControlAction {
	public:
		virtual ~AbstractControlAction() = default;
		virtual bool PopIsTriggering() = 0;
		virtual void Execute() = 0;
	};

	class ButtonKeyAction : public AbstractControlAction
	{
	public:
		enum class TriggerType {
			triggerOnPress,
			triggerContinuosly,
			triggerOnRelease
		};
		ButtonKeyAction() = default;

		void Init(int _key, TriggerType trigger = TriggerType::triggerOnPress) {
			key = _key;
			triggerType = trigger;
		}

		virtual ~ButtonKeyAction() = default;

		int getKey() const;

		void OnPress(int _key, int _scancode, int _mods);
		void OnRelease(int _key, int _scancode, int _mods);
		bool PopIsTriggering() override;

		void SetAction(std::function<void()> fun) {
			action = fun;
		}

		void Execute() override {
			action();
		}

	private:
		int key;
		bool pressed = false;
		bool pressedPreviously = false;
		std::function<void()> action = []() {};

		TriggerType triggerType;
	};

	class AxisMoveAction : public AbstractControlAction {
	public:
		AxisMoveAction() = default;

		virtual ~AxisMoveAction() = default;

		bool PopIsTriggering();

		void OnAxisMove(const glm::vec2& _pixPos, bool isFirst = false);

		void SetAction(std::function<void(const glm::vec2& pixDelta, const glm::vec2& pixPos)> fun) {
			action = fun;
		}

		void Execute() override {
			action(pixDelta, pixPos);
		}

	private:
		glm::vec2 pixDelta;
		glm::vec2 pixPos;
		glm::vec2 cumulatedDelta;
		bool movedInThisFrame;
		std::function<void(const glm::vec2& pixDelta, const glm::vec2& pixPos)> action = [](const glm::vec2& pixDelta, const glm::vec2& pixPos) {};
	};

	/*
	* 	class ClickOnScreen : public ButtonKeyAction {
	public:
		ClickOnScreen() : ButtonKeyAction(GLFW_MOUSE_BUTTON_LEFT, TriggerType::triggerOnPress) {}
	};

	class ReleaseClickOnScreen : public ButtonKeyAction {
	public:
		ReleaseClickOnScreen() : ButtonKeyAction(GLFW_MOUSE_BUTTON_LEFT, TriggerType::triggerOnRelease) {}
	};

	class GrabAction : public ButtonKeyAction {
	public:
		GrabAction() : ButtonKeyAction(GLFW_MOUSE_BUTTON_RIGHT, TriggerType::triggerOnPress) {}
	};

	class ReleaseAction : public ButtonKeyAction {
	public:
		ReleaseAction() : ButtonKeyAction(GLFW_MOUSE_BUTTON_RIGHT, TriggerType::triggerOnRelease) {}
	};

	class MoveAvatarForward : public ButtonKeyAction {
	public:
		MoveAvatarForward() : ButtonKeyAction(GLFW_KEY_W, ButtonKeyAction::TriggerType::triggerContinuosly) {

		}

		
	};

	class MoveAvatarBackward : public ButtonKeyAction {
	public:
		MoveAvatarBackward() : ButtonKeyAction(GLFW_KEY_S, ButtonKeyAction::TriggerType::triggerContinuosly) {
		}

		
	};

	class MoveAvatarRight : public ButtonKeyAction {
	public:
		MoveAvatarRight() : ButtonKeyAction(GLFW_KEY_D, ButtonKeyAction::TriggerType::triggerContinuosly) {
		}

		
	};

	class MoveAvatarLeft : public ButtonKeyAction {
	public:
		MoveAvatarLeft() : ButtonKeyAction(GLFW_KEY_A, ButtonKeyAction::TriggerType::triggerContinuosly) {
		}

		
	};

	class MoveAvatarUp : public ButtonKeyAction {
	public:
		MoveAvatarUp() : ButtonKeyAction(GLFW_KEY_E, ButtonKeyAction::TriggerType::triggerContinuosly) {
		}

		
	};

	class MoveAvatarDown : public ButtonKeyAction {
	public:
		MoveAvatarDown() : ButtonKeyAction(GLFW_KEY_Q, ButtonKeyAction::TriggerType::triggerContinuosly) {
		}

		
	};

	class JumpAvatar : public ButtonKeyAction {
	public:
		JumpAvatar() : ButtonKeyAction(GLFW_KEY_SPACE, ButtonKeyAction::TriggerType::triggerOnPress) {
		}

		
	};

	class ToggleGUI : public ButtonKeyAction {
	public:
		ToggleGUI() : ButtonKeyAction(GLFW_KEY_O) {
		}

		
	};

	class ToggleHUD : public ButtonKeyAction {
	public:
		ToggleHUD() : ButtonKeyAction(GLFW_KEY_H) {
		}

		
	};

	class StepFeature : public ButtonKeyAction {
	public:
		StepFeature() : ButtonKeyAction(GLFW_KEY_SPACE) {
		}

		
	};

	class FastForward : public ButtonKeyAction {
	public:
		FastForward() : ButtonKeyAction(GLFW_KEY_F, ButtonKeyAction::TriggerType::triggerContinuosly) {
		}

		
	};

	class Rewind : public ButtonKeyAction {
	public:
		Rewind() : ButtonKeyAction(GLFW_KEY_R, ButtonKeyAction::TriggerType::triggerContinuosly) {
		}

		
	};

	class TogglePause : public ButtonKeyAction {
	public:
		TogglePause() : ButtonKeyAction(GLFW_KEY_P) {
		}

		
	};

	class ToggleFullScreenMode : public ButtonKeyAction {
	public:
		ToggleFullScreenMode() : ButtonKeyAction(GLFW_KEY_TAB) {
		}

		
	};

	class ToggleDebugInfo : public ButtonKeyAction {
	public:
		ToggleDebugInfo() : ButtonKeyAction(GLFW_KEY_I) {
		}

		
	};

	class PrimaryAction : public ButtonKeyAction {
	public:
		PrimaryAction() : ButtonKeyAction(GLFW_MOUSE_BUTTON_LEFT, TriggerType::triggerContinuosly) {
		}

		
	};

	class SecondaryAction : public ButtonKeyAction {
	public:
		SecondaryAction() : ButtonKeyAction(GLFW_MOUSE_BUTTON_RIGHT, TriggerType::triggerContinuosly) {
		}

		
	};

	class RestartAction : public ButtonKeyAction {
	public:
		RestartAction() : ButtonKeyAction(GLFW_KEY_R) {
		}

		
	};

	class QuitAction : public ButtonKeyAction {
	public:
		QuitAction() : ButtonKeyAction(GLFW_KEY_ESCAPE) {
		}

		
	};
	*/
}