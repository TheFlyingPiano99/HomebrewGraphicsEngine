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
			for (const auto& func : deleteAllFunctions) {
				func();
			}
		}

		static void AddDeleteAllFunction(const std::function<void()>& func) {
			deleteAllFunctions.push_back(func);
		}

		static void IncrementAllocationCount() {
			currentAllocationCount++;
		}

		static void DecrementAllocationCount() {
			currentAllocationCount--;
		}
		
		static int GetCurrentAllocationCount() {
			return currentAllocationCount;
		}

	private:
			
		static std::vector<std::function<void()>> deleteAllFunctions;
		static int currentAllocationCount;
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
			instances.push_back(instance);
			MasterAllocator::IncrementAllocationCount();
			return instance;
		}

		static void Delete(T*& instance) {
			if (auto iter = std::ranges::find(instances.begin(), instances.end(), instance); iter != instances.end()) {
				instances.erase(iter);
				delete instance;
				MasterAllocator::DecrementAllocationCount();
			}
			instance = nullptr;
		}

		static void DeleteAll() {
			for (T* instance : instances) {
				delete instance;
				MasterAllocator::DecrementAllocationCount();
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