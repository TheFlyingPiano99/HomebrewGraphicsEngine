#include "ControlActionManager.h"


ControlActionManager* ControlActionManager::instance = nullptr;

void ControlActionManager::onKey(int _key, int _scancode, int _action, int _mods)
{
	for (auto contrAct : registeredActions) {
		if (contrAct->isThisAction(_key, _scancode, _action)) {
			queuedActions.push(contrAct);
		}
	}
}

void ControlActionManager::clearQueue()
{
	while (!queuedActions.empty()) {
		queuedActions.pop();
	}
}

void ControlActionManager::registerDefault()
{
	registerAction(new MoveCameraForward());
	registerAction(new MoveCameraBackward());
	registerAction(new ToggleGUI());
	registerAction(new FastForward());
	registerAction(new Rewind());
	registerAction(new TogglePause());
	registerAction(new ToggleFullScreenMode());
}

void ControlActionManager::registerAction(ControlAction* toRegister)
{
	registeredActions.push_back(toRegister);
}

void ControlActionManager::deregisterAction(ControlAction* toDeregister)
{
	registeredActions.erase(std::find(registeredActions.begin(), registeredActions.end(), toDeregister));
}

ControlAction* ControlActionManager::popNextQueuedAction()
{
	if (!queuedActions.empty()) {
		ControlAction* toReturn = queuedActions.front();
		queuedActions.pop();
		return toReturn;
	}
	return nullptr;
}
