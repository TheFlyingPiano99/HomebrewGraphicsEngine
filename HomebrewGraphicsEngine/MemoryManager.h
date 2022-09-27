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
			if (instances.empty()) {	// Heuristic solution: if the array of instances get cleared multiple lambdas get passed to the master allocator!
				MasterAllocator::AddDeleteAllFunction(Allocator<T>::DeleteAll);
			}
			auto* instance = new T();
			instances.push_back(instance);
			return instance;
		}

		static void Delete(T*& instance) {
			instances.erase(std::find(instances.begin(), instances.end(), instance));
			delete instance;
			instance = nullptr;
		}

		static void DeleteAll() {
			for (T* instance : instances) {
				delete instance;
			}
			instances.clear();
			std::cout << "Delete all of type " << std::endl;
		}

	private:
		static std::vector<T*> instances;
	};

	template<typename T>
	std::vector<T*> Allocator<T>::instances = std::vector<T*>();


}