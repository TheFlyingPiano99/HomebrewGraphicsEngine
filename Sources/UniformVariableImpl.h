#pragma once
#include "UniformVariable.h"
#include "MemoryManager.h"
#include <string>

namespace Hogra {
	template<typename T>
	class UniformVariable : public AbstractUniformVariable
	{
		ALLOCATOR_CONSTRUCTIBLE
	public:
		UniformVariable() = default;

		void Init(std::string_view _key, const T& _val) {
			key = _key;
			value = _val;
		}

		void Bind(unsigned int programID) override;

		void Set(const T val);

		T Get();

		T& operator*() {
			return value;
		}

		const T& operator*() const {
			return value;
		}

	private:
		T value;
	};
}
