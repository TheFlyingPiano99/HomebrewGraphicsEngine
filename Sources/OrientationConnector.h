#pragma once
#include "OrientationProvider.h"


namespace Hogra {
	class OrientationConnector {
	public:
		explicit OrientationConnector() = default;

		void Init(OrientationProvider* _provider, const glm::quat& _offset = glm::angleAxis(0.0f, glm::vec3(0,1,0))) {
			provider = _provider;
			offset = _offset;
		}

		glm::quat GetOrientation() const {
			return provider->GetOrientation() * offset;
		}

	private:
		OrientationProvider* provider;
		glm::quat offset;
	};
}