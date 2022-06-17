#pragma once
#include <glm/ext/quaternion_float.hpp>

namespace hograengine {
	class OrientationProvider
	{
	public:
		virtual const glm::quat& getOrientation() const = 0;
	};
}
