#include "ControlActionManager.h"

namespace Hogra {

	ControlActionManager* ControlActionManager::instance = nullptr;

	void ControlActionManager::onPress(const int _key, const int _scancode, const int _mods)
	{
		auto iter = registeredKeyActions.find(_key);
		if (iter != registeredKeyActions.end()) {
			iter->second->onPress(_key, _scancode, _mods);
		}
	}

	void ControlActionManager::onRelease(const int _key, const int _scancode, const int _mods)
	{
		auto iter = registeredKeyActions.find(_key);
		if (iter != registeredKeyActions.end()) {
			iter->second->onRelease(_key, _scancode, _mods);
		}
	}

	void ControlActionManager::OnMouseLeftButtonPress(const glm::vec2& ndcCoords)
	{
		if (nullptr != leftMouseButtonAction) {
			leftMouseButtonAction->onPress(GLFW_MOUSE_BUTTON_LEFT, 0, 0);
		}
	}

	void ControlActionManager::OnMouseLeftButtonRelease(const glm::vec2& ndcCoords)
	{
		if (nullptr != leftMouseButtonAction) {
			leftMouseButtonAction->onRelease(GLFW_MOUSE_BUTTON_LEFT, 0, 0);
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
		for (auto& pair : registeredKeyActions) {
			if (pair.second->isTriggering()) {
				queuedActions.push(pair.second);
			}
		}
		if (nullptr!= leftMouseButtonAction && leftMouseButtonAction->isTriggering()) {
			queuedActions.push(leftMouseButtonAction);
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
		registerAction(new TogglePause());
		registerAction(new ToggleFullScreenMode());
		registerAction(new JumpAvatar());
		registerAction(new ToggleDebugInfo());

		leftMouseButtonAction = new PrimaryAction();
	}

	void ControlActionManager::registerAction(ControlAction* toRegister)
	{
		registeredKeyActions.emplace(toRegister->getKey(), toRegister);
	}

	void ControlActionManager::deregisterAction(ControlAction* toDeregister)
	{
		registeredKeyActions.erase(toDeregister->getKey());
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

	void ControlActionManager::DestroyInstance() {
		if (nullptr != instance) {
			delete instance;
			instance = nullptr;
		}
	}
}