#pragma once
#include <vector>

namespace Hogra {

	class Component
	{
	public:

		static void Deallocate(Component* instance)
		{
			auto iter = std::find(heapAllocatedInstances.begin(), heapAllocatedInstances.end(), instance);
			if (iter != heapAllocatedInstances.end()) {
				heapAllocatedInstances.erase(iter);
				delete instance;
			}
		}

		static void DeallocateAll() {
			for (auto& instance : heapAllocatedInstances) {
				delete instance;
			}
			heapAllocatedInstances.clear();
		}


		virtual ~Component() = default;

		virtual void BeforePhysicsLoopUpdate() {}

		virtual void EarlyPhysicsUpdate(float dt) {};

		virtual void Update() {};

		virtual void LatePhysicsUpdate(float dt) {};

		virtual void AfterPhysicsLoopUpdate() {}

	protected:
		static std::vector<Component*> heapAllocatedInstances;
	};

}