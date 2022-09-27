#include "Light.h"
#include<glm/gtc/matrix_transform.hpp>
#include "Scene.h"
namespace Hogra {
	Light* Light::Instantiate()
	{
		auto* instance = new Light();
		heapAllocatedInstances.push_back(instance);
		return instance;
	}

	void Light::Init(const glm::vec4& _position, const glm::vec3& _powerDensity)
	{
		this->position = _position;
		this->powerDensity = _powerDensity;
		effectiveRadius = getEffectiveRadius();
		volumeModelMatrix = glm::translate(glm::vec3(GetPosition())) * glm::scale(glm::vec3(effectiveRadius, effectiveRadius, effectiveRadius));
	}
	void Light::ExportData(UniformBufferObject& ubo, unsigned int& idx) {
		ubo.UploadSubData(glm::value_ptr(position), idx++);
		ubo.UploadSubData(glm::value_ptr(powerDensity), idx++);
	}

	void Light::EarlyPhysicsUpdate(float dt)
	{
	}

	void Light::Update()
	{
		if (positionProvider != nullptr) {
			position = glm::vec4(positionProvider->GetPosition(), 1.0f);
		}
		volumeModelMatrix = glm::translate(glm::vec3(position)) * glm::scale(glm::vec3(effectiveRadius, effectiveRadius, effectiveRadius));
	}
	
	float Light::getEffectiveRadius() const {
		float lightMax = std::fmaxf(std::fmaxf(powerDensity.r, powerDensity.g), powerDensity.b);
		return std::sqrtf(4.0f * lightMax * 256.0f / 5.0f) / 2.0f;
	}
}