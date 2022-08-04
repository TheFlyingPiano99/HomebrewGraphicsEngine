#pragma once
#include <vector>

namespace Hogra {

	class MemoryManager {
	public:
		/*
		* Needs to be called before closing program!
		*/
		static void DeallocateAll();

		static void Deallocate(void* instance);

		static std::vector<void*> heapAllocatedInstances;

	};

}