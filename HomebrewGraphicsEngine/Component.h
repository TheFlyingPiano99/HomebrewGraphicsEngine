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

		virtual void PreUserInputControl(float dt) {}

		virtual void Control(float dt) = 0;

		virtual void Update(float dt) = 0;

	protected:
		static std::vector<Component*> heapAllocatedInstances;
	};

}