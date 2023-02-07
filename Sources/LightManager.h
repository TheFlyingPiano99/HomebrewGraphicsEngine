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

		void initDefferedSystem(unsigned int _contextWidth, unsigned int _contextHeight) {
			deferredLightingSystem.Init(_contextWidth, _contextHeight);
		}

		void OnContextResize(unsigned int contextWidth, unsigned int contextHeight) {
			deferredLightingSystem.OnContextResize(contextWidth, contextHeight);
		}

		void RenderDeferredLighting();

		void BindGBuffer() {
			deferredLightingSystem.BindGBuffer();
		}

		void initDebug();

		void drawDebug();

		void Clear();
		
		void Update();

		FBO& GetGBuffer() {
			return deferredLightingSystem.GetGBuffer();
		}

		const Texture2D& GetDepthTexture();

	private:
		std::vector<Light*> lights;
		UniformBufferObject ubo;
		DeferredLightingSystem deferredLightingSystem;
		Mesh* debugLightVolumeMesh = nullptr;
		ShaderProgram glyphProgram;
		Material debugMaterial;
		Geometry* debugGeometry = nullptr;
	};
}

