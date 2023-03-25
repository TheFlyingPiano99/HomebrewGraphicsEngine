#pragma once
#include <vector>
#include <iostream>
#include <set>
#include <functional>
#include <numeric>

#define ALLOCATOR_CONSTRUCTIBLE private: inline void* operator new(std::size_t size) { return ::operator new(size); } friend class Allocator;


namespace Hogra {

	template<class T, class... Args>
	concept not_publicly_heap_allocatable = !requires (Args... args) {
		new T(args...);
	};

	static class Allocator {
	public:
		struct AllocationData {
			mutable void* pointer;
			std::function<void()> deleteFunc;

			std::strong_ordering operator<=>(const AllocationData& ad) const {
				return pointer <=> ad.pointer;
			}
		};

		template<typename AllocationType, typename... Args>
			requires std::constructible_from<AllocationType, Args...> 
				&& not_publicly_heap_allocatable<AllocationType, Args...>
				&& requires(Args... args) { new AllocationType(args...); }	// can use new in Allocator scope
				&& std::destructible<AllocationType>
		static AllocationType* New(Args... args) {
			auto* p = new AllocationType(args...);
			allocations.insert(AllocationData{ p, [p]() { delete p; } });
			return p;
		}

		static void Delete(std::destructible auto*& p) {
			AllocationData toFind;
			toFind.pointer = p;
			if (
				auto iterator = allocations.find(toFind);
				allocations.end() != iterator && nullptr != iterator->pointer
			) {
				iterator->deleteFunc();
				iterator->pointer = nullptr;
				allocations.erase(iterator);
			}
			p = nullptr;
		}

		static bool IsValid(std::destructible auto* p) {
			AllocationData toFind;
			toFind.pointer = p;
			auto iterator = allocations.find(toFind);
			return allocations.end() != iterator && nullptr != iterator->pointer;
		}

		static void DeleteAll() {
			for (auto& allocation : allocations) {
				if (allocation.pointer) {
					allocation.deleteFunc();
					allocation.pointer = nullptr;
				}
			}
			allocations.clear();
		}

		static int GetAllocationCount() {
			
			return std::accumulate(
				allocations.begin(), 
				allocations.end(),
				0, 
				[](int sum, const AllocationData& data) { 
					return (nullptr != data.pointer) ? sum + 1 : sum; 
				}
			);
		}

	private:
		static std::set<AllocationData> allocations;
	};
}