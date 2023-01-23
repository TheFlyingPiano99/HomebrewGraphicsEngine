#pragma once
#include <glm/ext/vector_float3.hpp>

namespace Hogra {
	class ScaleProvider
	{
	public:
		virtual const glm::vec3& GetScale() const = 0;
	};
}

