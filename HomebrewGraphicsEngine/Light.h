#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderProgram.h"
#include "UniformBuffer.h"
#include "Component.h"
#include "PositionProvider.h"

namespace Hogra {

	class Light : public Component {
	public:
		Light(glm::vec4 position, glm::vec3 powerDensity);

		~Light() = default;

		void ExportData(UniformBufferObject& ubo, unsigned int& idx);

		// Inherited via Component
		void Control(float dt);

		void Update(float dt);

		PositionProvider* getPositionProvider() const {
			return positionProvider;
		}

		void SetPositionProvider(PositionProvider* provider) {
			positionProvider = provider;
		}

		const glm::vec4& GetPosition() const {
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