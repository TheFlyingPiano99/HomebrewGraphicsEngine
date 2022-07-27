#pragma once
#include <vector>
#include "Light.h"
#include "UniformBuffer.h"
#include "DeferredLightingSystem.h" 

namespace Hogra {
	class LightManager
	{
	public:
		LightManager();
		~LightManager();
		void ExportData();
		
		void AddLight(Light* _light) {
			lights.push_back(_light);
		}

		void initDefferedSystem(int contextWidth, int contextHeight) {
			deferredLightingSystem.Init(contextWidth, contextHeight);
		}

		void onResize(int contextWidth, int contextHeight) {
			deferredLightingSystem.Resize(contextWidth, contextHeight);
		}

		void renderDeferredLighting() {
			deferredLightingSystem.Draw(lights);
		}

		void BindGBuffer() {
			deferredLightingSystem.BindGBuffer();
		}

		void initDebug();

		void drawDebug();

		void Clear();

	private:
		std::vector<Light*> lights;
		UniformBufferObject ubo;
		DeferredLightingSystem deferredLightingSystem;
		Mesh* debugLightVolumeMesh = nullptr;
		ShaderProgram* shaderProgram = nullptr;
		Material* debugMaterial = nullptr;
		Geometry* debugGeometry = nullptr;
	};
}

