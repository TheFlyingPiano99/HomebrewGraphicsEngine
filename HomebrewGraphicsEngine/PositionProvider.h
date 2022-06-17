#pragma once
#include <glm/ext/vector_float3.hpp>

namespace hograengine {
	class PositionProvider
	{
	public:

		virtual const glm::vec3& getPosition() const = 0;

	};
}

