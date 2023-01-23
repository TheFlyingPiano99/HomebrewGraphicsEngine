#pragma once
#include "Component.h"
#include <functional>

namespace Hogra {
	class ScriptObject : public Component
	{
	public:

		void BeforePhysicsLoopUpdate() override { beforePhysicsLoopUpdateFunc(); }
		void EarlyPhysicsUpdate(float dt) override { earlyPhysicsUpdateFunc(); };
		void LatePhysicsUpdate(float dt) override { latePhysicsUpdateFunc(); };
		void AfterPhysicsLoopUpdate() override { afterPhysicsLoopUpdateFunc(); }

		void SetBeforePhysicsLoopUpdateFunc(std::function<void()> func) { beforePhysicsLoopUpdateFunc = func; }
		void SetEarlyPhysicsUpdateFunc(std::function<void()> func) { earlyPhysicsUpdateFunc = func; }
		void SetLatePhysicsUpdateFunc(std::function<void()> func) { latePhysicsUpdateFunc = func; }
		void SetAfterPhysicsLoopUpdateFunc(std::function<void()> func) { afterPhysicsLoopUpdateFunc = func; }

	private:
		std::function<void()> beforePhysicsLoopUpdateFunc = []() {};
		std::function<void()> earlyPhysicsUpdateFunc = []() {};
		std::function<void()> latePhysicsUpdateFunc = []() {};
		std::function<void()> afterPhysicsLoopUpdateFunc = []() {};
	};
}

