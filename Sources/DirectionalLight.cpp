#include "DirectionalLight.h"
#include<glm/gtc/matrix_transform.hpp>
#include "Scene.h"


namespace Hogra {

	constexpr int noShadowMap = -1;

	void DirectionalLight::Init(const glm::vec3& _direction, const glm::vec3& _powerDensity)
	{
		this->direction = glm::vec4(_direction, 0.0);
		this->direction3D = _direction;
		this->powerDensity = _powerDensity;
	}

	void DirectionalLight::ExportData(UniformBufferObject& ubo, unsigned int& idx) {
		ubo.UploadSubData(glm::value_ptr(direction), idx++);
		ubo.UploadSubData(glm::value_ptr(powerDensity), idx++);
		ubo.UploadSubData((void*)((nullptr != shadowCaster) ? &(shadowCaster->GetIdx()) : &noShadowMap), idx++);
	}

	void DirectionalLight::EarlyPhysicsUpdate(float dt)
	{
	}

	void DirectionalLight::LatePhysicsUpdate(float dt)
	{
	}
}