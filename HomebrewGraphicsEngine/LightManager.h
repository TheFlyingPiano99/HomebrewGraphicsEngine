#pragma once
#include <vector>
#include "Light.h"
#include "UniformBuffer.h"

namespace hograengine {
	class LightManager
	{
	public:
		LightManager();
		~LightManager();
		void exportData();
		void registerLights(const std::vector<Light*>& _lights) {
			lights = _lights;
		}

	private:
		std::vector<Light*> lights;
		UniformBufferObject* ubo = nullptr;
	};
}

