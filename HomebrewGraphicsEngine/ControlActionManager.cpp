#include "ControlActionManager.h"
#include "MemoryManager.h"

namespace Hogra {

	ControlActionManager* ControlActionManager::instance = nullptr;

	void ControlActionManager::OnPress(const int _key, const int _scancode, const int _mods)
	{
		for (auto iterPair = registeredKeyActions.equal_range(_key); iterPair.first != iterPair.second; iterPair.first++) {
			iterPair.first->second->OnPress(_key, _scancode, _mods);
		}
	}

	void ControlActionManager::OnRelease(const int _key, const int _scancode, const int _mods)
	{
		for (auto iterPair = registeredKeyActions.equal_range(_key); iterPair.first != iterPair.second; iterPair.first++) {
			iterPair.first->second->OnRelease(_key, _scancode, _mods);
		}
	}

	void ControlActionManager::OnMouseButtonPress(const int _key, const glm::vec2& pixCoords)
	{
		for (auto iterPair = registeredMouseButtonActions.equal_range(_key); iterPair.first != iterPair.second; iterPair.first++) {
			iterPair.first->second->OnPress(_key, 0, 0);
		}
	}

	void ControlActionManager::OnMouseButtonRelease(const int _key, const glm::vec2& pixCoords)
	{
		for (auto iterPair = registeredMouseButtonActions.equal_range(_key); iterPair.first != iterPair.second; iterPair.first++) {
			iterPair.first->second->OnRelease(_key, 0, 0);
		}
	}

	void ControlActionManager::OnMouseMove(const glm::vec2& pixPos, bool isFirst)
	{
		for (auto& action : mouseMoveAction) {
			action->OnAxisMove(pixPos, isFirst);
		}
	}

	void ControlActionManager::OnMouseScroll(float x)
	{
		for (auto& action : mouseScrollAction) {
			action->OnAxisMove(glm::vec2(x));
		}
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
		for (auto action : registeredMouseButtonActions) {
			if (action.second->PopIsTriggering()) {
				queuedActions.push(action.second);
			}
		}
		for (auto action : mouseMoveAction) {
			if (action->PopIsTriggering()) {
				queuedActions.push(action);
			}
		}
		for (auto action : mouseScrollAction) {
			if (action->PopIsTriggering()) {
				queuedActions.push(action);
			}
		}
	}

	void ControlActionManager::UnregisterControls() {
		ClearQueue();
		for (auto& act : registeredKeyActions) {
			auto p = act.second;
			Allocator::Delete(p);
		}
		registeredKeyActions.clear();
		for (auto act : registeredMouseButtonActions) {
			auto p = act.second;
			Allocator::Delete(p);
		}
		registeredMouseButtonActions.clear();
		for (auto act : mouseMoveAction) {
			Allocator::Delete(act);
		}
		mouseMoveAction.clear();
		for (auto act : mouseScrollAction) {
			Allocator::Delete(act);
		}
		mouseScrollAction.clear();
	}

	void ControlActionManager::RegisterKeyAction(ButtonKeyAction* toRegister)
	{
		registeredKeyActions.emplace(toRegister->getKey(), toRegister);
	}

	void ControlActionManager::UnregisterKeyAction(ButtonKeyAction* toDeregister)
	{
		auto&& range = registeredKeyActions.equal_range(toDeregister->getKey());
		for (; range.first != range.second; range.first++) {
			if (range.first->second == toDeregister) {
				registeredKeyActions.erase(range.first);
				break;
			}
		}
	}

	void ControlActionManager::RegisterMouseButtonAction(ButtonKeyAction* toRegister) {
		registeredMouseButtonActions.emplace(toRegister->getKey(), toRegister);
	}

	void ControlActionManager::UnregisterMouseButtonAction(ButtonKeyAction* toDeregister) {
		auto&& range = registeredMouseButtonActions.equal_range(toDeregister->getKey());
		for (; range.first != range.second; range.first++) {
			if (range.first->second == toDeregister) {
				registeredKeyActions.erase(range.first);
				break;
			}
		}
	}

	void ControlActionManager::RegisterMouseMoveAction(AxisMoveAction* toRegister) {
		mouseMoveAction.push_back(toRegister);
	}

	void ControlActionManager::UnregisterMouseMoveAction(AxisMoveAction* toDeregister) {
		auto iter = std::find(mouseMoveAction.begin(), mouseMoveAction.end(), toDeregister);
		mouseMoveAction.erase(iter);
	}

	void ControlActionManager::RegisterMouseScrollAction(AxisMoveAction* toRegister) {
		mouseScrollAction.push_back(toRegister);
	}

	void ControlActionManager::UnregisterMouseScrollAction(AxisMoveAction* toDeregister) {
		auto iter = std::find(mouseScrollAction.begin(), mouseScrollAction.end(), toDeregister);
		mouseScrollAction.erase(iter);
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
			instance = Allocator::New<ControlActionManager>();
		}
		return instance;
	}

	void ControlActionManager::DestroyInstance() {
		Allocator::Delete(instance);
	}
}