#include "Light.h"
#include<glm/gtc/matrix_transform.hpp>
#include "Scene.h"
namespace hograengine {
	Light::Light(glm::vec4 position, glm::vec3 powerDensity)
		: position(position), powerDensity(powerDensity) {
		effectiveRadius = getEffectiveRadius();
		volumeModelMatrix = glm::translate(glm::vec3(getPosition())) * glm::scale(glm::vec3(effectiveRadius, effectiveRadius, effectiveRadius));
	}
	void Light::exportData(UniformBufferObject* ubo, unsigned int& idx) {
		ubo->uploadSubData(glm::value_ptr(position), idx++);
		ubo->uploadSubData(glm::value_ptr(powerDensity), idx++);
	}
	void Light::control(float dt)
	{
	}

	void Light::update(float dt)
	{
		if (positionProvider != nullptr) {
			position = glm::vec4(positionProvider->getPosition(), 1.0f);
			volumeModelMatrix = glm::translate(glm::vec3(position)) * glm::scale(glm::vec3(effectiveRadius, effectiveRadius, effectiveRadius));
		}
	}
	
	float Light::getEffectiveRadius() const {
		float lightMax = std::fmaxf(std::fmaxf(powerDensity.r, powerDensity.g), powerDensity.b);
		return std::sqrtf(4.0f * lightMax * 256.0f / 5.0f) / 2.0f;
	}
}