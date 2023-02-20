#pragma once
#include <vector>
#include "PointLight.h"
#include "DirectionalLight.h"
#include "UniformBuffer.h"
#include "DeferredLightingSystem.h" 

namespace Hogra {
	class LightManager
	{
	public:
		LightManager();

		~LightManager();

		void ExportData(const std::vector<OmniDirectionalShadowCaster*>& omniDirShadowCasters);
		
		void AddLight(PointLight* _light) {
			pointLights.push_back(_light);
		}

		void AddLight(DirectionalLight* _light) {
			dirLights.push_back(_light);
		}

		void InitDefferedSystem(unsigned int _contextWidth, unsigned int _contextHeight) {
			deferredLightingSystem.Init(_contextWidth, _contextHeight);
		}

		void OnContextResize(unsigned int contextWidth, unsigned int contextHeight) {
			deferredLightingSystem.OnContextResize(contextWidth, contextHeight);
		}

		void RenderDeferredLighting();

		void BindGBuffer() {
			deferredLightingSystem.BindGBuffer();
		}

		void InitDebug();

		void DrawDebug();

		void Clear();
		
		void Update();

		FBO& GetGBuffer() {
			return deferredLightingSystem.GetGBuffer();
		}

		const Texture2D& GetDepthTexture();

	private:
		std::vector<PointLight*> pointLights;
		std::vector<DirectionalLight*> dirLights;
		UniformBufferObject ubo;
		DeferredLightingSystem deferredLightingSystem;
		Mesh* debugLightVolumeMesh = nullptr;
		ShaderProgram glyphProgram;
		Material debugMaterial;
		Geometry* debugGeometry = nullptr;
	};
}

