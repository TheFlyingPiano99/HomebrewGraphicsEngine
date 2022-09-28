#pragma once
#include <vector>
#include "MemoryManager.h"

namespace Hogra {

	class Component
	{
		friend class Allocator<Component>;
	public:

		virtual ~Component() = default;

		virtual void BeforePhysicsLoopUpdate() {}

		virtual void EarlyPhysicsUpdate(float dt) {};

		virtual void Update() {};

		virtual void LatePhysicsUpdate(float dt) {};

		virtual void AfterPhysicsLoopUpdate() {}

	protected:
	};

}