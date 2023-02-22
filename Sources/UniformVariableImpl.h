#pragma once
#include "UniformVariable.h"

namespace Hogra {
	template<typename T>
	class UniformVariable : public AbstractUniformVariable
	{
	public:
		UniformVariable() = default;

		void Init(const char* _key, const T& _val) {
			key = _key;
			value = _val;
		}

		void Bind(unsigned int id) override;

		void Set(const T val);

		const T Get();

	private:
		const char* key;
		T value;
	};
}
