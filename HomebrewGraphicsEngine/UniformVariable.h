#pragma once
#include "ShaderProgram.h"

namespace Hogra {

	class AbstractUniformVariable {
	public:
		AbstractUniformVariable() = default;
		virtual ~AbstractUniformVariable() = default;
		virtual void Bind(const ShaderProgram& program) = 0;
	};
		
	template<typename T>
	class UniformVariable : public AbstractUniformVariable
	{
	public:
		UniformVariable() = default;

		void Init(const char* _key, const T& val) {
			key = _key;
			value = val;
		}

		void Bind(const ShaderProgram& program) override;

		void Set(const T val);

		const T Get();

	private:
		const char* key;
		T value;
	};
}
