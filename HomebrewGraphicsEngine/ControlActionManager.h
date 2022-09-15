#pragma once

#include "ControlAction.h"
#include <map>
#include <iostream>
namespace Hogra {

	class ControlActionManager
	{
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

		void OnMouseLeftButtonPress(const glm::vec2& pixCoords);

		void OnMouseLeftButtonRelease(const glm::vec2& pixCoords);

		void OnMouseRightButtonPress(const glm::vec2& pixCoords);

		void OnMouseRightButtonRelease(const glm::vec2& pixCoords);

		void OnMouseMove(const glm::vec2& pixCoords);

		/*
		* Register new controlAction
		*/
		void RegisterAction(ButtonKeyAction* toRegister);

		/*
		* Unregister controlAction
		*/
		void UnregisterAction(ButtonKeyAction* toDeregister);

		/*
		* Remove all queued actions
		*/
		void ClearQueue();

		/*
		* Push on the execution queue the currently triggering control actions.
		* Should be called from main loop.
		*/
		void QueueTriggeringActions();


		/*
		* Execute queued actions
		*/
		void ExecuteQueue(Scene& scene) {
			auto* action = PopNextQueuedAction();
			while (nullptr != action) {
				action->Execute(scene);
				action = PopNextQueuedAction();
			}
		}

		/*
		* Init default control scheme
		*/
		void RegisterDefault();

		void SetCursorVisible(bool b);

		bool IsCursorVisible();

	private:
		static ControlActionManager* instance;
		std::map<const int, ButtonKeyAction*> registeredKeyActions;
		ButtonKeyAction* leftMouseButtonAction = nullptr;
		ButtonKeyAction* rightMouseButtonAction = nullptr;
		AxisMoveAction* mouseMoveAction = nullptr;
		bool isCursorVisible = false;

		std::queue<AbstractControlAction*> queuedActions;

		ControlActionManager() = default;

		~ControlActionManager() {
			ClearQueue();
			for (auto& act : registeredKeyActions) {
				delete act.second;
			}
			registeredKeyActions.clear();
			if (nullptr != leftMouseButtonAction) {
				delete leftMouseButtonAction;
			}
			if (nullptr != rightMouseButtonAction) {
				delete rightMouseButtonAction;
			}
			if (nullptr != mouseMoveAction) {
				delete mouseMoveAction;
			}
		}

		/*
		* Get next action for execution
		*/
		AbstractControlAction* PopNextQueuedAction();
	};

}