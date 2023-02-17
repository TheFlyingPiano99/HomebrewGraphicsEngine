#pragma once
#include "Component.h"
#include <functional>

namespace Hogra {
	class ScriptObject : public Component
	{
	public:

		void BeforePhysicsLoopUpdate() override { beforePhysicsLoopUpdateFunc(); }
		void EarlyPhysicsUpdate(float dt) override { earlyPhysicsUpdateFunc(dt); };
		void LatePhysicsUpdate(float dt) override { latePhysicsUpdateFunc(dt); };
		void AfterPhysicsLoopUpdate() override { afterPhysicsLoopUpdateFunc(); }

		void SetBeforePhysicsLoopUpdateFunc(std::function<void()> func) { beforePhysicsLoopUpdateFunc = func; }
		void SetEarlyPhysicsUpdateFunc(std::function<void(float)> func) { earlyPhysicsUpdateFunc = func; }
		void SetLatePhysicsUpdateFunc(std::function<void(float)> func) { latePhysicsUpdateFunc = func; }
		void SetAfterPhysicsLoopUpdateFunc(std::function<void()> func) { afterPhysicsLoopUpdateFunc = func; }

	private:
		std::function<void()> beforePhysicsLoopUpdateFunc = []() {};
		std::function<void(float)> earlyPhysicsUpdateFunc = [](float _) {};
		std::function<void(float)> latePhysicsUpdateFunc = [](float _) {};
		std::function<void()> afterPhysicsLoopUpdateFunc = []() {};
	};
}

