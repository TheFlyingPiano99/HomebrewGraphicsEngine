#pragma once

#include "ControlAction.h"
class ControlActionManager
{
	static ControlActionManager* instance;
	std::vector<ControlAction*> registeredActions;
	std::queue<ControlAction*> queuedActions;
	ControlActionManager() {
	}

	~ControlActionManager() {
		clearQueue();
		for (auto act : registeredActions) {
			delete act;
		}
		registeredActions.clear();
	}

	/*
	* Get next action for execution
	*/
	ControlAction* popNextQueuedAction();

public:

	static ControlActionManager* getInstance() {
		if (nullptr == instance) {
			instance = new ControlActionManager();
		}
		return instance;
	}

	static void destroyInstance() {
		if (nullptr != instance) {
			delete instance;
			instance = nullptr;
		}
	}

	/*
	* Process key event
	*/
	void onKey(int _key, int _scancode, int _action, int _mods);
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

};

