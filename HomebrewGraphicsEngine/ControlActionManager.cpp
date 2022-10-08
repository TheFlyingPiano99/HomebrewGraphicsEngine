#include "ControlActionManager.h"
#include "MemoryManager.h"

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
		RegisterAction(Allocator<MoveAvatarForward>::New());
		RegisterAction(Allocator<MoveAvatarBackward>::New());
		RegisterAction(Allocator<MoveAvatarRight>::New());
		RegisterAction(Allocator<MoveAvatarLeft>::New());
		RegisterAction(Allocator<MoveAvatarUp>::New());
		RegisterAction(Allocator<MoveAvatarDown>::New());
		RegisterAction(Allocator<ToggleGUI>::New());
		RegisterAction(Allocator<TogglePause>::New());
		RegisterAction(Allocator<ToggleFullScreenMode>::New());
		RegisterAction(Allocator<JumpAvatar>::New());
		RegisterAction(Allocator<ToggleDebugInfo>::New());
		RegisterAction(Allocator<RestartAction>::New());
		RegisterAction(Allocator<QuitAction>::New());

		pressLeftMouseButtonAction = Allocator<PrimaryAction>::New();
		pressRightMouseButtonAction = Allocator<SecondaryAction>::New();
		releaseLeftMouseButtonAction = nullptr;
		releaseRightMouseButtonAction = nullptr;
		mouseMoveAction = Allocator<CameraMoveAction>::New();
		mouseScrollAction = Allocator<CameraZoomAction>::New();

	}

	void ControlActionManager::RegisterDefaultForVoxelObserving()
	{
		RegisterAction(Allocator<MoveAvatarForward>::New());
		RegisterAction(Allocator<MoveAvatarBackward>::New());
		RegisterAction(Allocator<MoveAvatarRight>::New());
		RegisterAction(Allocator<MoveAvatarLeft>::New());
		RegisterAction(Allocator<MoveAvatarUp>::New());
		RegisterAction(Allocator<MoveAvatarDown>::New());
		RegisterAction(Allocator<ToggleGUI>::New());
		RegisterAction(Allocator<ToggleHUD>::New());
		RegisterAction(Allocator<TogglePause>::New());
		RegisterAction(Allocator<ToggleFullScreenMode>::New());
		RegisterAction(Allocator<JumpAvatar>::New());
		RegisterAction(Allocator<ToggleDebugInfo>::New());
		RegisterAction(Allocator<RestartAction>::New());
		RegisterAction(Allocator<QuitAction>::New());

		pressLeftMouseButtonAction = Allocator<ClickOnScreen>::New();
		pressRightMouseButtonAction = Allocator<GrabAction>::New();
		releaseLeftMouseButtonAction = nullptr;
		releaseRightMouseButtonAction = Allocator<ReleaseAction>::New();

		mouseMoveAction = Allocator<CameraMoveAction>::New();
		mouseScrollAction = Allocator<CameraZoomAction>::New();

	}

	void ControlActionManager::UnregisterControls() {
		ClearQueue();
		for (auto& act : registeredKeyActions) {
			Allocator<ButtonKeyAction>::Delete(act.second);
		}
		registeredKeyActions.clear();
		Allocator<ButtonKeyAction>::Delete(pressLeftMouseButtonAction);
		Allocator<ButtonKeyAction>::Delete(pressRightMouseButtonAction);
		Allocator<ButtonKeyAction>::Delete(releaseLeftMouseButtonAction);
		Allocator<ButtonKeyAction>::Delete(releaseRightMouseButtonAction);
		Allocator<AxisMoveAction>::Delete(mouseMoveAction);
		Allocator<AxisMoveAction>::Delete(mouseScrollAction);
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
			instance = Allocator<ControlActionManager>::New();
		}
		return instance;
	}

	void ControlActionManager::DestroyInstance() {
		Allocator<ControlActionManager>::Delete(instance);
	}
}