#pragma once
#include "glad/glad.h"
#include <memory>
#include <vector>

namespace Hogra {

	class Texture
	{
	public:
		GLuint ID;
		GLuint unit;

		static void Deallocate(Texture* instance)
		{
			if (nullptr == instance) {
				return;
			}
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

		virtual ~Texture() = default;

		virtual void Bind() const = 0;

		virtual void Unbind() const = 0;
		
		void Delete() const;

	protected:
		inline void* operator new(std::size_t size) { return ::operator new(size); }

		Texture() = default;
		static std::vector<Texture*> heapAllocatedInstances;
	};

}