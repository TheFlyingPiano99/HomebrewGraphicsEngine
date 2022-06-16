#pragma once

#include "Scene.h"
#include <queue>
#include <GLFW/glfw3.h>
namespace hograengine {

	class ControlAction
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
		ControlAction(int _key, TriggerType trigger = TriggerType::triggerOnPress) : key(_key), triggerType(trigger) {
		}
		virtual ~ControlAction() = default;

		int getKey() const;

		void onPress(int _key, int _scancode, int _mods);
		void onRelease(int _key, int _scancode, int _mods);
		bool isTriggering();

		virtual void execute(Scene* scene, float dt) = 0;
	};

	class MoveCameraForward : public ControlAction {
	public:
		MoveCameraForward() : ControlAction(GLFW_KEY_W, ControlAction::TriggerType::triggerContinuosly) {

		}

		void execute(Scene* scene, float dt) override;
	};

	class MoveCameraBackward : public ControlAction {
	public:
		MoveCameraBackward() : ControlAction(GLFW_KEY_S, ControlAction::TriggerType::triggerContinuosly) {
		}

		void execute(Scene* scene, float dt) override;
	};

	class MoveCameraRight : public ControlAction {
	public:
		MoveCameraRight() : ControlAction(GLFW_KEY_D, ControlAction::TriggerType::triggerContinuosly) {
		}

		void execute(Scene* scene, float dt) override;
	};

	class MoveCameraLeft : public ControlAction {
	public:
		MoveCameraLeft() : ControlAction(GLFW_KEY_A, ControlAction::TriggerType::triggerContinuosly) {
		}

		void execute(Scene* scene, float dt) override;
	};

	class MoveCameraUp : public ControlAction {
	public:
		MoveCameraUp() : ControlAction(GLFW_KEY_E, ControlAction::TriggerType::triggerContinuosly) {
		}

		void execute(Scene* scene, float dt) override;
	};

	class MoveCameraDown : public ControlAction {
	public:
		MoveCameraDown() : ControlAction(GLFW_KEY_Q, ControlAction::TriggerType::triggerContinuosly) {
		}

		void execute(Scene* scene, float dt) override;
	};

	class ToggleGUI : public ControlAction {
	public:
		ToggleGUI() : ControlAction(GLFW_KEY_O) {
		}

		void execute(Scene* scene, float dt) override;
	};

	class FastForward : public ControlAction {
	public:
		FastForward() : ControlAction(GLFW_KEY_F, ControlAction::TriggerType::triggerContinuosly) {
		}

		void execute(Scene* scene, float dt) override;
	};

	class Rewind : public ControlAction {
	public:
		Rewind() : ControlAction(GLFW_KEY_R, ControlAction::TriggerType::triggerContinuosly) {
		}

		void execute(Scene* scene, float dt) override;
	};

	class TogglePause : public ControlAction {
	public:
		TogglePause() : ControlAction(GLFW_KEY_P) {
		}

		void execute(Scene* scene, float dt) override;
	};

	class ToggleFullScreenMode : public ControlAction {
	public:
		ToggleFullScreenMode() : ControlAction(GLFW_KEY_TAB) {
		}

		void execute(Scene* scene, float dt) override;
	};
}