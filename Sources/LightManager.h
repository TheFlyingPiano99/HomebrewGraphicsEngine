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

		void OnResize(int contextWidth, int contextHeight) {
			deferredLightingSystem.Resize(contextWidth, contextHeight);
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
		ShaderProgram shaderProgram;
		Material debugMaterial;
		Geometry* debugGeometry = nullptr;
	};
}

