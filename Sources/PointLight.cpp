#include "PointLight.h"
#include<glm/gtc/matrix_transform.hpp>
#include "Scene.h"


namespace Hogra {

	constexpr int noShadowMap = -1;

	void PointLight::Init(const glm::vec3& _position, const glm::vec3& _powerDensity)
	{
		this->position = glm::vec4(_position, 1.0f);
		this->position3D = _position;
		this->powerDensity = _powerDensity;
		effectiveRadius = getEffectiveRadius();
		volumeModelMatrix = glm::translate(position3D) * glm::scale(glm::vec3(effectiveRadius, effectiveRadius, effectiveRadius));
	}

	void PointLight::ExportData(UniformBufferObject& ubo, unsigned int& idx) {
		ubo.UploadSubData(glm::value_ptr(position), idx++);
		ubo.UploadSubData(glm::value_ptr(powerDensity), idx++);
		ubo.UploadSubData((void*)((nullptr != shadowCaster)? &(shadowCaster->GetIdx()) : &noShadowMap), idx++);
	}

	void PointLight::EarlyPhysicsUpdate(float dt)
	{
	}

	void PointLight::LatePhysicsUpdate(float dt)
	{
		if (positionConnector != nullptr) {
			position3D = glm::vec4(positionConnector->GetPosition(), 1.0f);
			position.x = position3D.x;
			position.y = position3D.y;
			position.z = position3D.z;
		}
		volumeModelMatrix = glm::translate(position3D) * glm::scale(glm::vec3(effectiveRadius, effectiveRadius, effectiveRadius));
	}
	
	constexpr float effectiveDownscale = 0.4f;

	float PointLight::getEffectiveRadius() const {
		float lightMax = std::fmaxf(std::fmaxf(powerDensity.r, powerDensity.g), powerDensity.b);
		return std::sqrtf(4.0f * lightMax * 256.0f / 5.0f) / 2.0f * effectiveDownscale;
	}
}