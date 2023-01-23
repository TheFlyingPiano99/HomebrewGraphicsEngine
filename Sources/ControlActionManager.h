#pragma once

#include "ControlAction.h"
#include <map>
#include <iostream>
#include "MemoryManager.h"

namespace Hogra {

	class ControlActionManager
	{
		friend class Allocator;
	public:

		static ControlActionManager* getInstance();

		static void DestroyInstance();

		/*
		* Process key event
		*/
		void OnPress(const int _key, const int _scancode, const int _mods);

		/*
		* Process key event
		*/
		void OnRelease(const int _key, const int _scancode, const int _mods);

		void OnMouseButtonPress(const int _key, const glm::vec2& pixCoords);

		void OnMouseButtonRelease(const int _key, const glm::vec2& pixCoords);

		void OnMouseMove(const glm::vec2& pixPos, bool isFirst);

		void OnMouseScroll(float delta);

		void RegisterKeyAction(ButtonKeyAction* toRegister);

		void UnregisterKeyAction(ButtonKeyAction* toDeregister);

		void RegisterMouseButtonAction(ButtonKeyAction* toRegister);

		void UnregisterMouseButtonAction(ButtonKeyAction* toDeregister);

		void RegisterMouseMoveAction(AxisMoveAction* toRegister);

		void UnregisterMouseMoveAction(AxisMoveAction* toDeregister);

		void RegisterMouseScrollAction(AxisMoveAction* toRegister);

		void UnregisterMouseScrollAction(AxisMoveAction* toDeregister);

		/*
		* Remove all queued actions
		*/
		void ClearQueue();

		/*
		* Push on the execution queue the currently triggering control actions.
		* Should be called from main loop once per frame.
		*/
		void QueueTriggeringActions();

		/*
		* Execute queued actions
		*/
		void ExecuteQueue(Scene& scene) {
			auto* action = PopNextQueuedAction();
			while (nullptr != action) {
				action->Execute();
				action = PopNextQueuedAction();
			}
		}

		void UnregisterControls();


	private:
		static ControlActionManager* instance;
		std::multimap<const int, ButtonKeyAction*> registeredKeyActions;
		std::multimap<const int, ButtonKeyAction*> registeredMouseButtonActions;
		std::vector<AxisMoveAction*> mouseMoveAction;
		std::vector<AxisMoveAction*> mouseScrollAction;

		std::queue<AbstractControlAction*> queuedActions;

		ControlActionManager() = default;

		~ControlActionManager() {
			UnregisterControls();
		}

		/*
		* Get next action for execution
		*/
		AbstractControlAction* PopNextQueuedAction();
	};

}