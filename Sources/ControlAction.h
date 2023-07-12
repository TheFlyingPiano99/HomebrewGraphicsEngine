#pragma once

#include "Scene.h"
#include <queue>
#include <GLFW/glfw3.h>

namespace Hogra {

	class AbstractControlAction {
		ALLOCATOR_CONSTRUCTIBLE
	public:
		virtual ~AbstractControlAction() = default;
		virtual bool PopIsTriggering() = 0;
		virtual void Execute(Scene* scene) = 0;
	};

	class ButtonKeyAction : public AbstractControlAction
	{
		ALLOCATOR_CONSTRUCTIBLE
	public:
		enum class TriggerType {
			triggerOnPress,
			triggerContinuosly,
			triggerOnRelease
		};
		ButtonKeyAction() = default;

		void Init(int _key, TriggerType trigger = TriggerType::triggerOnPress) {
			key = _key;
			triggerType = trigger;
		}

		virtual ~ButtonKeyAction() = default;

		int getKey() const;

		void OnPress(int _key, int _scancode, int _mods);
		void OnRelease(int _key, int _scancode, int _mods);
		bool PopIsTriggering() override;

		void SetAction(std::function<void()> fun) {
			action = fun;
		}

		void Execute(Scene* scene) override {
			action();
		}

	private:
		int key;
		bool pressed = false;
		bool pressedPreviously = false;
		std::function<void()> action = []() {};

		TriggerType triggerType;
	};

	class AxisMoveAction : public AbstractControlAction {
		ALLOCATOR_CONSTRUCTIBLE
	public:
		AxisMoveAction() = default;

		virtual ~AxisMoveAction() = default;

		bool PopIsTriggering();

		void OnAxisMove(const glm::vec2& _pixPos, bool isFirst = false);

		void SetAction(std::function<void(const glm::vec2& pixDelta, const glm::vec2& pixPos)> fun) {
			action = fun;
		}

		void Execute(Scene* scene) override {
			action(pixDelta, pixPos);
		}

	private:
		glm::vec2 pixDelta;
		glm::vec2 pixPos;
		glm::vec2 cumulatedDelta;
		bool movedInThisFrame;
		std::function<void(const glm::vec2& pixDelta, const glm::vec2& pixPos)> action = [](const glm::vec2& pixDelta, const glm::vec2& pixPos) {};
	};

	class CursorClickAction : public AbstractControlAction
	{
		ALLOCATOR_CONSTRUCTIBLE
	public:

		void OnPress(const glm::ivec2& screenCursorPos);

		void OnRelease(const glm::ivec2& screenCursorPos);

		bool PopIsTriggering() override;

		void Execute(Scene* scene) override {
			scene->OnCursorClick(screenCursorPos);
		}

	private:
		bool pressed = false;
		bool pressedPreviously = false;
		glm::ivec2 screenCursorPos;
	};

}



