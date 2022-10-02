#pragma once
#include <glm/ext/vector_float3.hpp>
#include "PositionProvider.h"


namespace Hogra {
	class PositionConnector {
	public:
		explicit PositionConnector() = default;

		void Init(PositionProvider* _provider, const glm::vec3& _offset = glm::vec3(0.0f)) {
			provider = _provider;
			offset = _offset;
		}

		glm::vec3 GetPosition() const {
			return provider->GetPosition() + offset;
		}

	private:
		PositionProvider* provider;
		glm::vec3 offset = glm::vec3(0.0f);
	};
}