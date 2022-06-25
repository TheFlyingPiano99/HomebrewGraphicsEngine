#include "Light.h"
#include<glm/gtc/matrix_transform.hpp>
#include "Scene.h"
namespace hograengine {

	void Light::exportData(UniformBufferObject* ubo, unsigned int& idx) {
		ubo->uploadSubData(glm::value_ptr(position), idx++);
		ubo->uploadSubData(glm::value_ptr(powerDensity), idx++);
	}
}