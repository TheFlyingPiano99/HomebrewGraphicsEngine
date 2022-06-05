#pragma once

#include "ControlAction.h"
#include <map>
#include <iostream>

class ControlActionManager
{
public:

	static ControlActionManager* getInstance();

	static void destroyInstance();

	/*
	* Process key event
	*/
	void onPress(const int _key, const int _scancode, const int _mods);

	/*
	* Process key event
	*/
	void onRelease(const int _key, const int _scancode, const int _mods);

	/*
	* Register new controlAction
	*/
	void registerAction(ControlAction* toRegister);

	/*
	* Deregister controlAction
	*/
	void deregisterAction(ControlAction* toDeregister);

	/*
	* Remove all queued actions
	*/
	void clearQueue();

	/*
	* Push on the execution queue the currently triggering control actions.
	* Should be called from main loop.
	*/
	void queueTriggeringActions();


	/*
	* Execute queued actions
	*/
	void executeQueue(Scene* scene, float dt) {
		ControlAction* action = popNextQueuedAction();
		while (nullptr != action) {
			action->execute(scene, dt);
			action = popNextQueuedAction();
		}
	}

	/*
	* Init default control scheme
	*/
	void registerDefault();

private:
	static ControlActionManager* instance;
	std::map<const int, ControlAction*> registeredActions;
	std::queue<ControlAction*> queuedActions;

	ControlActionManager() = default;

	~ControlActionManager() {
		clearQueue();
		for (auto& act : registeredActions) {
			delete act.second;
		}
		registeredActions.clear();
	}

	/*
	* Get next action for execution
	*/
	ControlAction* popNextQueuedAction();
};

