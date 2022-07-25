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
		Light(glm::vec4 position, glm::vec3 powerDensity);

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

		const glm::vec4& getPosition() const {
			return position;
		}

		const glm::vec3& getPowerDensity() const {
			return powerDensity;
		}

		float getEffectiveRadius() const;

		const glm::mat4& getVolumeModelMatrix() const {
			return volumeModelMatrix;
		}

	private:
		glm::vec4 position;
		glm::vec3 powerDensity;
		glm::mat4 volumeModelMatrix;
		PositionProvider* positionProvider;
		float effectiveRadius;
	};
}