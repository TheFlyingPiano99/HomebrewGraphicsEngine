#pragma once
#include <glm/ext/vector_float3.hpp>

namespace Hogra {
	class PositionProvider
	{
	public:

		virtual const glm::vec3& GetPosition() const = 0;

	};
}

