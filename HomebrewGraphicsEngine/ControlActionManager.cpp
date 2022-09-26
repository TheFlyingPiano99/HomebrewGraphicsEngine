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
		if (nullptr != pressLeftMouseButtonAction) {
			pressLeftMouseButtonAction->OnPress(GLFW_MOUSE_BUTTON_LEFT, 0, 0);
		}
		if (nullptr != releaseLeftMouseButtonAction) {
			releaseLeftMouseButtonAction->OnPress(GLFW_MOUSE_BUTTON_LEFT, 0, 0);
		}
	}

	void ControlActionManager::OnMouseLeftButtonRelease(const glm::vec2& pixCoords)
	{
		if (nullptr != pressLeftMouseButtonAction) {
			pressLeftMouseButtonAction->OnRelease(GLFW_MOUSE_BUTTON_LEFT, 0, 0);
		}
		if (nullptr != releaseLeftMouseButtonAction) {
			releaseLeftMouseButtonAction->OnRelease(GLFW_MOUSE_BUTTON_LEFT, 0, 0);
		}
	}

	void ControlActionManager::OnMouseRightButtonPress(const glm::vec2& pixCoords)
	{
		if (nullptr != pressRightMouseButtonAction) {
			pressRightMouseButtonAction->OnPress(GLFW_MOUSE_BUTTON_RIGHT, 0, 0);
		}
		if (nullptr != releaseRightMouseButtonAction) {
			releaseRightMouseButtonAction->OnPress(GLFW_MOUSE_BUTTON_RIGHT, 0, 0);
		}
	}

	void ControlActionManager::OnMouseRightButtonRelease(const glm::vec2& pixCoords)
	{
		if (nullptr != pressRightMouseButtonAction) {
			pressRightMouseButtonAction->OnRelease(GLFW_MOUSE_BUTTON_RIGHT, 0, 0);
		}
		if (nullptr != releaseRightMouseButtonAction) {
			releaseRightMouseButtonAction->OnRelease(GLFW_MOUSE_BUTTON_RIGHT, 0, 0);
		}
	}

	void ControlActionManager::OnMouseMove(const glm::vec2& pixPos, bool isFirst)
	{
		if (nullptr == mouseMoveAction) {
			return;
		}
		mouseMoveAction->OnAxisMove(pixPos, isFirst);
	}

	void ControlActionManager::OnMouseScroll(float x)
	{
		mouseScrollAction->OnAxisMove(glm::vec2(x));
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
		if (nullptr!= pressLeftMouseButtonAction && pressLeftMouseButtonAction->PopIsTriggering()) {
			queuedActions.push(pressLeftMouseButtonAction);
		}
		if (nullptr != pressRightMouseButtonAction && pressRightMouseButtonAction->PopIsTriggering()) {
			queuedActions.push(pressRightMouseButtonAction);
		}
		if (nullptr != releaseLeftMouseButtonAction && releaseLeftMouseButtonAction->PopIsTriggering()) {
			queuedActions.push(releaseLeftMouseButtonAction);
		}
		if (nullptr != releaseRightMouseButtonAction && releaseRightMouseButtonAction->PopIsTriggering()) {
			queuedActions.push(releaseRightMouseButtonAction);
		}
		if (nullptr != mouseMoveAction && mouseMoveAction->PopIsTriggering()) {
			queuedActions.push(mouseMoveAction);
		}
		if (nullptr != mouseScrollAction && mouseScrollAction->PopIsTriggering()) {
			queuedActions.push(mouseScrollAction);
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

		pressLeftMouseButtonAction = new PrimaryAction();
		pressRightMouseButtonAction = new SecondaryAction();
		mouseMoveAction = new CameraMoveAction();
		mouseScrollAction = new CameraZoomAction();

	}

	void ControlActionManager::RegisterDefaultForVoxelObserving()
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

		pressLeftMouseButtonAction = new PrimaryAction();
		pressRightMouseButtonAction = new GrabAction();

		releaseLeftMouseButtonAction = nullptr;		// Not used
		releaseRightMouseButtonAction = new ReleaseAction();

		mouseMoveAction = new CameraMoveAction();
		mouseScrollAction = new CameraZoomAction();

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