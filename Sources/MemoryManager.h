#pragma once
#include <vector>
#include <iostream>
#include <set>
#include <functional>
#include <numeric>

namespace Hogra {

	class Allocator {
	public:
		struct AllocationData {
			mutable void* pointer;
			std::function<void()> deleteFunc;

			bool operator==(const AllocationData& ad) {
				return reinterpret_cast<uintptr_t>(pointer) == reinterpret_cast<uintptr_t>(ad.pointer);
			}

			bool operator<(const AllocationData& ad) {
				return reinterpret_cast<uintptr_t>(pointer) < reinterpret_cast<uintptr_t>(ad.pointer);
			}

			bool operator>(const AllocationData& ad) {
				return reinterpret_cast<uintptr_t>(pointer) > reinterpret_cast<uintptr_t>(ad.pointer);
			}
		};


		template<typename T>
		static T* New() {
			auto* p = new T();
			allocations.insert(AllocationData{ p, [p]() { delete p; } });
			return p;
		}

		template<typename T>
		static void Delete(T*& p) {
			auto toFind = AllocationData();
			toFind.pointer = p;
			if (
				auto iterator = allocations.find(toFind);
				allocations.end() != iterator && nullptr != iterator->pointer
				) {
				iterator->deleteFunc();
				iterator->pointer = nullptr;
			}
			p = nullptr;
		}


		static void DeleteAll() {
			for (auto& allocation : allocations) {
				if (nullptr != allocation.pointer) {
					allocation.deleteFunc();
					allocation.pointer = nullptr;
				}
			}
			allocations.clear();
		}

		static int GetAllocationCount() {
			
			return std::accumulate(allocations.begin(), allocations.end(), 0, [](int sum, const AllocationData& data) { return (nullptr != data.pointer) ? sum + 1 : sum; });
		}

	private:
		static std::set<AllocationData> allocations;
	};

	bool operator<(const Allocator::AllocationData& ad, const Allocator::AllocationData& ad2);

	bool operator>(const Allocator::AllocationData& ad, const Allocator::AllocationData& ad2);
}