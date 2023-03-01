#pragma once
#include "UniformVariable.h"
#include <string>

namespace Hogra {
	template<typename T>
	class UniformVariable : public AbstractUniformVariable
	{
	public:
		UniformVariable() = default;

		void Init(std::string_view _key, const T& _val) {
			key = _key;
			value = _val;
		}

		void Bind(unsigned int id) override;

		void Set(const T val);

		const T Get();

	private:
		std::string key;
		T value;
	};
}
