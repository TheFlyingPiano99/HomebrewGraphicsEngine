#include "LightManager.h"
#include "GlobalInclude.h"

namespace hograengine {
	LightManager::LightManager()
	{
		std::vector<int> subDataSizes;
		subDataSizes.push_back(sizeof(int));			// lightCount
		for (int i = 0; i < MAX_LIGHT_COUNT; i++) {
			subDataSizes.push_back(sizeof(glm::vec4));		// position
			subDataSizes.push_back(sizeof(glm::vec3));		// powerDensity
		}
		ubo = new UniformBufferObject(subDataSizes, LIGHTS_UBO_BINDING);
	}

	LightManager::~LightManager()
	{
		delete ubo;
	}

	void LightManager::exportData()
	{
		int count = lights.size();
		ubo->Bind();
		unsigned int idx = 0;
		ubo->uploadSubData((void*)&count, idx++);
		for (auto* light : lights) {
			light->exportData(ubo, idx);
		}
		ubo->Unbind();
	}
}
