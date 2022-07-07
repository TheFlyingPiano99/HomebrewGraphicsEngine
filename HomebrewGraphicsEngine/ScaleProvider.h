#pragma once
#include <glm/ext/vector_float3.hpp>

namespace hograengine {
	class ScaleProvider
	{
	public:
		virtual const glm::vec3& getScale() const = 0;
	};
}

