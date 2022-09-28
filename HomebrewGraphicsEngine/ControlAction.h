#pragma once

#include "Scene.h"
#include <queue>
#include <GLFW/glfw3.h>
namespace Hogra {

	class AbstractControlAction {
	public:
		virtual ~AbstractControlAction() = default;
		virtual bool PopIsTriggering() = 0;

		virtual void Execute(Scene& scene) = 0;
	};

	class ButtonKeyAction : public AbstractControlAction
	{
	protected:
		enum class TriggerType {
			triggerOnPress,
			triggerContinuosly,
			triggerOnRelease
		};

	private:
		int key;
		bool pressed = false;
		bool pressedPreviously = false;

		TriggerType triggerType;

	public:
		ButtonKeyAction(int _key, TriggerType trigger = TriggerType::triggerOnPress) : key(_key), triggerType(trigger) {
		}

		virtual ~ButtonKeyAction() = default;

		int getKey() const;

		void OnPress(int _key, int _scancode, int _mods);
		void OnRelease(int _key, int _scancode, int _mods);
		bool PopIsTriggering() override;

	};

	class AxisMoveAction : public AbstractControlAction {
	public:
		AxisMoveAction() = default;

		virtual ~AxisMoveAction() = default;

		bool PopIsTriggering();

		void OnAxisMove(const glm::vec2& _pixPos, bool isFirst = false);

	protected:
		glm::vec2 pixDelta;
		glm::vec2 pixPos;
		glm::vec2 cumulatedDelta;
		bool movedInThisFrame;
	};

	class CameraMoveAction : public AxisMoveAction {
	public:
		CameraMoveAction() : AxisMoveAction() {}

		void Execute(Scene& scene) override;
	};

	class CameraZoomAction : public AxisMoveAction {
	public:
		CameraZoomAction() : AxisMoveAction() {}

		void Execute(Scene& scene) override;
	};

	class ClickOnScreen : public ButtonKeyAction {
	public:
		ClickOnScreen() : ButtonKeyAction(GLFW_MOUSE_BUTTON_LEFT, TriggerType::triggerOnPress) {}

		void Execute(Scene& scene) override;
	};

	class GrabAction : public ButtonKeyAction {
	public:
		GrabAction() : ButtonKeyAction(GLFW_MOUSE_BUTTON_RIGHT, TriggerType::triggerOnPress) {}

		void Execute(Scene& scene) override;
	};

	class ReleaseAction : public ButtonKeyAction {
	public:
		ReleaseAction() : ButtonKeyAction(GLFW_MOUSE_BUTTON_RIGHT, TriggerType::triggerOnRelease) {}

		void Execute(Scene& scene) override;
	};

	class MoveAvatarForward : public ButtonKeyAction {
	public:
		MoveAvatarForward() : ButtonKeyAction(GLFW_KEY_W, ButtonKeyAction::TriggerType::triggerContinuosly) {

		}

		void Execute(Scene& scene) override;
	};

	class MoveAvatarBackward : public ButtonKeyAction {
	public:
		MoveAvatarBackward() : ButtonKeyAction(GLFW_KEY_S, ButtonKeyAction::TriggerType::triggerContinuosly) {
		}

		void Execute(Scene& scene) override;
	};

	class MoveAvatarRight : public ButtonKeyAction {
	public:
		MoveAvatarRight() : ButtonKeyAction(GLFW_KEY_D, ButtonKeyAction::TriggerType::triggerContinuosly) {
		}

		void Execute(Scene& scene) override;
	};

	class MoveAvatarLeft : public ButtonKeyAction {
	public:
		MoveAvatarLeft() : ButtonKeyAction(GLFW_KEY_A, ButtonKeyAction::TriggerType::triggerContinuosly) {
		}

		void Execute(Scene& scene) override;
	};

	class MoveAvatarUp : public ButtonKeyAction {
	public:
		MoveAvatarUp() : ButtonKeyAction(GLFW_KEY_E, ButtonKeyAction::TriggerType::triggerContinuosly) {
		}

		void Execute(Scene& scene) override;
	};

	class MoveAvatarDown : public ButtonKeyAction {
	public:
		MoveAvatarDown() : ButtonKeyAction(GLFW_KEY_Q, ButtonKeyAction::TriggerType::triggerContinuosly) {
		}

		void Execute(Scene& scene) override;
	};

	class JumpAvatar : public ButtonKeyAction {
	public:
		JumpAvatar() : ButtonKeyAction(GLFW_KEY_SPACE, ButtonKeyAction::TriggerType::triggerOnPress) {
		}

		void Execute(Scene& scene) override;
	};

	class ToggleGUI : public ButtonKeyAction {
	public:
		ToggleGUI() : ButtonKeyAction(GLFW_KEY_O) {
		}

		void Execute(Scene& scene) override;
	};

	class ToggleHUD : public ButtonKeyAction {
	public:
		ToggleHUD() : ButtonKeyAction(GLFW_KEY_H) {
		}

		void Execute(Scene& scene) override;
	};

	class FastForward : public ButtonKeyAction {
	public:
		FastForward() : ButtonKeyAction(GLFW_KEY_F, ButtonKeyAction::TriggerType::triggerContinuosly) {
		}

		void Execute(Scene& scene) override;
	};

	class Rewind : public ButtonKeyAction {
	public:
		Rewind() : ButtonKeyAction(GLFW_KEY_R, ButtonKeyAction::TriggerType::triggerContinuosly) {
		}

		void Execute(Scene& scene) override;
	};

	class TogglePause : public ButtonKeyAction {
	public:
		TogglePause() : ButtonKeyAction(GLFW_KEY_P) {
		}

		void Execute(Scene& scene) override;
	};

	class ToggleFullScreenMode : public ButtonKeyAction {
	public:
		ToggleFullScreenMode() : ButtonKeyAction(GLFW_KEY_TAB) {
		}

		void Execute(Scene& scene) override;
	};

	class ToggleDebugInfo : public ButtonKeyAction {
	public:
		ToggleDebugInfo() : ButtonKeyAction(GLFW_KEY_I) {
		}

		void Execute(Scene& scene) override;
	};

	class PrimaryAction : public ButtonKeyAction {
	public:
		PrimaryAction() : ButtonKeyAction(GLFW_MOUSE_BUTTON_LEFT, TriggerType::triggerContinuosly) {
		}

		void Execute(Scene& scene) override;
	};

	class SecondaryAction : public ButtonKeyAction {
	public:
		SecondaryAction() : ButtonKeyAction(GLFW_MOUSE_BUTTON_RIGHT, TriggerType::triggerContinuosly) {
		}

		void Execute(Scene& scene) override;
	};

	class RestartAction : public ButtonKeyAction {
	public:
		RestartAction() : ButtonKeyAction(GLFW_KEY_R) {
		}

		void Execute(Scene& scene) override;
	};

	class QuitAction : public ButtonKeyAction {
	public:
		QuitAction() : ButtonKeyAction(GLFW_KEY_ESCAPE) {
		}

		void Execute(Scene& scene) override;
	};

}