#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderProgram.h"
#include "UniformBuffer.h"
#include "Component.h"
#include "PositionProvider.h"

namespace hograengine {

	class Light : public Component {
	public:
		Light(glm::vec4 position, glm::vec3 powerDensity)
			: position(position), powerDensity(powerDensity) {}

		~Light() = default;

		void exportData(UniformBufferObject* ubo, unsigned int& idx);

		// Inherited via Component
		void control(float dt);

		void update(float dt);

		PositionProvider* getPositionProvider() const {
			return positionProvider;
		}

		void setPositionProvider(PositionProvider* provider) {
			positionProvider = provider;
		}

	private:
		glm::vec4 position;
		glm::vec3 powerDensity;
		PositionProvider* positionProvider;
	};
}