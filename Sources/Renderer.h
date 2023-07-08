#pragma once
#include <vector>
#include "PointLight.h"
#include "DirectionalLight.h"
#include "UniformBuffer.h"
#include "DeferredLightingSystem.h" 

namespace Hogra {
	class Renderer
	{
	public:
		Renderer();

		~Renderer();

		void ExportData(const std::vector<OmniDirectionalShadowCaster*>& omniDirShadowCasters);
		
		void AddLight(PointLight* _light) {
			pointLights.push_back(_light);
		}

		void AddLight(DirectionalLight* _light) {
			dirLights.push_back(_light);
		}

		void SetSkybox(TextureCube* map);

		void InitDefferedSystem(unsigned int _contextWidth, unsigned int _contextHeight) {
			deferredLightingSystem.Init(_contextWidth, _contextHeight);
		}

		void OnContextResize(unsigned int contextWidth, unsigned int contextHeight);

		void RenderDeferredLighting();

		void BindGBuffer() {
			deferredLightingSystem.BindGBuffer();
		}

		void InitDebug();

		void DrawDebug();

		void Clear();
		
		void Update();

		void PrepareFirstFBOForRendering(FBO* fbo, const glm::vec4& backgroundColor);

		FBO& GetGBuffer() {
			return deferredLightingSystem.GetGBuffer();
		}

		const Texture2D& GetDepthTexture();

		void RenderOnScreen() const;

		FBO& GetFinalImageFBO() {
			return finalImageFBO;
		}

		void BlitOnScreen();

	private:
		std::vector<PointLight*> pointLights;
		std::vector<DirectionalLight*> dirLights;
		TextureCube* environmentMap = nullptr;
		TextureCube* irradianceMap = nullptr;
		TextureCube* prefilterMap = nullptr;
		UniformBufferObject lightsUBO;
		DeferredLightingSystem deferredLightingSystem;
		Mesh* debugLightVolumeMesh = nullptr;
		ShaderProgram glyphProgram;
		Material debugMaterial;
		Texture2D brdfLUT;
		Geometry* debugGeometry = nullptr;
		FBO finalImageFBO;
		Texture2D finalImage;
	};
}

