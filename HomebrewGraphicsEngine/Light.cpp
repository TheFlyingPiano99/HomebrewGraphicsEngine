#include "Light.h"
#include<glm/gtc/matrix_transform.hpp>
#include "Scene.h"
namespace Hogra {

	void Light::Init(const glm::vec4& _position, const glm::vec3& _powerDensity)
	{
		this->position = _position;
		this->position3D = _position;
		this->powerDensity = _powerDensity;
		effectiveRadius = getEffectiveRadius();
		volumeModelMatrix = glm::translate(position3D) * glm::scale(glm::vec3(effectiveRadius, effectiveRadius, effectiveRadius));
	}
	void Light::ExportData(UniformBufferObject& ubo, unsigned int& idx) {
		ubo.UploadSubData(glm::value_ptr(position), idx++);
		ubo.UploadSubData(glm::value_ptr(powerDensity), idx++);
	}

	void Light::EarlyPhysicsUpdate(float dt)
	{
	}

	void Light::LatePhysicsUpdate(float dt)
	{
		if (positionConnector != nullptr) {
			position3D = glm::vec4(positionConnector->GetPosition(), 1.0f);
			position.x = position3D.x;
			position.y = position3D.y;
			position.z = position3D.z;
		}
		volumeModelMatrix = glm::translate(position3D) * glm::scale(glm::vec3(effectiveRadius, effectiveRadius, effectiveRadius));
	}
	
	float Light::getEffectiveRadius() const {
		float lightMax = std::fmaxf(std::fmaxf(powerDensity.r, powerDensity.g), powerDensity.b);
		return std::sqrtf(4.0f * lightMax * 256.0f / 5.0f) / 2.0f;
	}
}