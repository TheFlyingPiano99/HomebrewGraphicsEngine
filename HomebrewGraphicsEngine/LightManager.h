#pragma once
#include <vector>
#include "Light.h"
#include "UniformBuffer.h"
#include "DeferredLightingSystem.h" 

namespace hograengine {
	class LightManager
	{
	public:
		LightManager();
		~LightManager();
		void exportData();
		
		void addLight(Light* _light) {
			lights.push_back(_light);
		}

		void initDefferedSystem(int contextWidth, int contextHeight) {
			deferredLightingSystem.Init(contextWidth, contextHeight);
		}

		void onResize(int contextWidth, int contextHeight) {
			deferredLightingSystem.Resize(contextWidth, contextHeight);
		}

		void renderDeferredLighting() {
			deferredLightingSystem.Draw();
		}

		void BindGBuffer() {
			deferredLightingSystem.BindGBuffer();
		}

	private:
		std::vector<Light*> lights;
		UniformBufferObject* ubo = nullptr;
		DeferredLightingSystem deferredLightingSystem;
	};
}

