#include "ControlActionManager.h"

namespace hograengine {

	ControlActionManager* ControlActionManager::instance = nullptr;

	void ControlActionManager::onPress(const int _key, const int _scancode, const int _mods)
	{
		auto iter = registeredActions.find(_key);
		if (iter != registeredActions.end()) {
			bool trigger = false;
			iter->second->onPress(_key, _scancode, _mods);
		}
	}

	void ControlActionManager::onRelease(const int _key, const int _scancode, const int _mods)
	{
		auto iter = registeredActions.find(_key);
		if (iter != registeredActions.end()) {
			bool trigger = false;
			iter->second->onRelease(_key, _scancode, _mods);
		}
	}

	void ControlActionManager::clearQueue()
	{
		while (!queuedActions.empty()) {
			queuedActions.pop();
		}
	}

	void ControlActionManager::queueTriggeringActions()
	{
		for (auto& pair : registeredActions) {
			if (pair.second->isTriggering()) {
				queuedActions.push(pair.second);
			}
		}
	}

	void ControlActionManager::registerDefault()
	{
		registerAction(new MoveAvatarForward());
		registerAction(new MoveAvatarBackward());
		registerAction(new MoveAvatarRight());
		registerAction(new MoveAvatarLeft());
		registerAction(new MoveAvatarUp());
		registerAction(new MoveAvatarDown());
		registerAction(new ToggleGUI());
		registerAction(new FastForward());
		registerAction(new Rewind());
		registerAction(new TogglePause());
		registerAction(new ToggleFullScreenMode());
		registerAction(new JumpAvatar());
		registerAction(new ToggleDebugInfo());
	}

	void ControlActionManager::registerAction(ControlAction* toRegister)
	{
		registeredActions.emplace(toRegister->getKey(), toRegister);
	}

	void ControlActionManager::deregisterAction(ControlAction* toDeregister)
	{
		registeredActions.erase(toDeregister->getKey());
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

	ControlActionManager* ControlActionManager::getInstance() {
		if (nullptr == instance) {
			instance = new ControlActionManager();
		}
		return instance;
	}

	void ControlActionManager::destroyInstance() {
		if (nullptr != instance) {
			delete instance;
			instance = nullptr;
		}
	}
}