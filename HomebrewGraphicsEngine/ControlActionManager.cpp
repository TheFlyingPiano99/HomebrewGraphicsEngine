#include "ControlActionManager.h"

namespace Hogra {

	ControlActionManager* ControlActionManager::instance = nullptr;

	void ControlActionManager::OnPress(const int _key, const int _scancode, const int _mods)
	{
		auto iter = registeredKeyActions.find(_key);
		if (iter != registeredKeyActions.end()) {
			iter->second->OnPress(_key, _scancode, _mods);
		}
	}

	void ControlActionManager::OnRelease(const int _key, const int _scancode, const int _mods)
	{
		auto iter = registeredKeyActions.find(_key);
		if (iter != registeredKeyActions.end()) {
			iter->second->OnRelease(_key, _scancode, _mods);
		}
	}

	void ControlActionManager::OnMouseLeftButtonPress(const glm::vec2& pixCoords)
	{
		if (nullptr != leftMouseButtonAction) {
			leftMouseButtonAction->OnPress(GLFW_MOUSE_BUTTON_LEFT, 0, 0);
		}
	}

	void ControlActionManager::OnMouseLeftButtonRelease(const glm::vec2& pixCoords)
	{
		if (nullptr != leftMouseButtonAction) {
			leftMouseButtonAction->OnRelease(GLFW_MOUSE_BUTTON_LEFT, 0, 0);
		}
	}

	void ControlActionManager::OnMouseRightButtonPress(const glm::vec2& pixCoords)
	{
		if (nullptr != rightMouseButtonAction) {
			rightMouseButtonAction->OnPress(GLFW_MOUSE_BUTTON_RIGHT, 0, 0);
		}
	}

	void ControlActionManager::OnMouseRightButtonRelease(const glm::vec2& pixCoords)
	{
		if (nullptr != rightMouseButtonAction) {
			rightMouseButtonAction->OnRelease(GLFW_MOUSE_BUTTON_RIGHT, 0, 0);
		}
	}

	void ControlActionManager::OnMouseMove(const glm::vec2& pixPos)
	{
		if (nullptr == mouseMoveAction) {
			return;
		}
		mouseMoveAction->OnMove(pixPos);
	}

	void ControlActionManager::ClearQueue()
	{
		while (!queuedActions.empty()) {
			queuedActions.pop();
		}
	}

	void ControlActionManager::QueueTriggeringActions()
	{

		// Keyboard:
		for (auto& pair : registeredKeyActions) {
			if (pair.second->PopIsTriggering()) {
				queuedActions.push(pair.second);
			}
		}

		// Mouse:
		if (nullptr!= leftMouseButtonAction && leftMouseButtonAction->PopIsTriggering()) {
			queuedActions.push(leftMouseButtonAction);
		}
		if (nullptr != rightMouseButtonAction && rightMouseButtonAction->PopIsTriggering()) {
			queuedActions.push(rightMouseButtonAction);
		}
		if (nullptr != mouseMoveAction && mouseMoveAction->PopIsTriggering()) {
			queuedActions.push(mouseMoveAction);
		}
	}

	void ControlActionManager::RegisterDefault()
	{
		RegisterAction(new MoveAvatarForward());
		RegisterAction(new MoveAvatarBackward());
		RegisterAction(new MoveAvatarRight());
		RegisterAction(new MoveAvatarLeft());
		RegisterAction(new MoveAvatarUp());
		RegisterAction(new MoveAvatarDown());
		RegisterAction(new ToggleGUI());
		RegisterAction(new TogglePause());
		RegisterAction(new ToggleFullScreenMode());
		RegisterAction(new JumpAvatar());
		RegisterAction(new ToggleDebugInfo());
		RegisterAction(new RestartAction());
		RegisterAction(new QuitAction());

		leftMouseButtonAction = new PrimaryAction();
		rightMouseButtonAction = new SecondaryAction();
		mouseMoveAction = new CameraMoveAction();
	}

	void ControlActionManager::SetCursorVisible(bool b)
	{
		isCursorVisible = b;
	}

	bool ControlActionManager::IsCursorVisible()
	{
		return isCursorVisible;
	}

	void ControlActionManager::RegisterAction(ButtonKeyAction* toRegister)
	{
		registeredKeyActions.emplace(toRegister->getKey(), toRegister);
	}

	void ControlActionManager::UnregisterAction(ButtonKeyAction* toDeregister)
	{
		registeredKeyActions.erase(toDeregister->getKey());
	}

	AbstractControlAction* ControlActionManager::PopNextQueuedAction()
	{
		if (!queuedActions.empty()) {
			AbstractControlAction* toReturn = queuedActions.front();
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