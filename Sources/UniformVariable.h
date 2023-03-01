#pragma once

namespace Hogra {

	class AbstractUniformVariable {
	public:
		AbstractUniformVariable() = default;
		virtual ~AbstractUniformVariable() = default;
		virtual void Bind(unsigned int id) = 0;
	};

}
