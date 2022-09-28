#pragma once
#include <vector>
#include <functional>
#include <iostream>

namespace Hogra {

	class MemoryManager {
	public:
		/*
		* Needs to be called before closing program!
		*/
		static void DeallocateAll();

		static void DeallocateSceneResources();

	};

	class MasterAllocator {
	public:
		static void DeleteAll() {
			for (auto& func : deleteAllFunctions) {
				func();
			}
		}

		static void AddDeleteAllFunction(const std::function<void()>& func) {
			deleteAllFunctions.push_back(func);
		}

	private:
		static std::vector<std::function<void()>> deleteAllFunctions;
	};

	template<typename T>
	class Allocator {
	public:

		static T* New() {
			if (isFirstAllocationOfThisType) {
				isFirstAllocationOfThisType = false;
				MasterAllocator::AddDeleteAllFunction(Allocator<T>::DeleteAll);
			}
			auto* instance = new T();
			std::cout << "New    " << instance << std::endl;
			instances.push_back(instance);
			return instance;
		}

		static void Delete(T*& instance) {
			if (auto iter = std::ranges::find(instances.begin(), instances.end(), instance); iter != instances.end()) {
				instances.erase(iter);
				std::cout << "Delete " << instance << std::endl;
				delete instance;
			}
			instance = nullptr;
		}

		static void DeleteAll() {
			for (T* instance : instances) {
				std::cout << "Delete " << instance << std::endl;
				delete instance;
			}
			instances.clear();
		}

	private:
		static std::vector<T*> instances;
		static bool isFirstAllocationOfThisType;
	};

	template<typename T>
	std::vector<T*> Allocator<T>::instances = std::vector<T*>();

	template<typename T>
	bool Allocator<T>::isFirstAllocationOfThisType = true;

}