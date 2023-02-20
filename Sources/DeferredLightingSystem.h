#pragma once
#include "FBO.h"
#include "AssetFolderPathManager.h"
#include "Material.h"
#include "Mesh.h"
#include "GeometryFactory.h"
#include "glm/gtx/transform.hpp"
#include "PointLight.h"
#include "DirectionalLight.h"

namespace Hogra {
	class DeferredLightingSystem
	{
	public:
		DeferredLightingSystem() = default;
			
		~DeferredLightingSystem();

		void Init(unsigned int _contextWidth, unsigned int _contextHeight);

		void OnContextResize(unsigned int _contextWidth, unsigned int _contextHeight);

		void BindGBuffer();

		void Draw(const std::vector<PointLight*>& pointLights, const std::vector<DirectionalLight*>& dirLights);

		const Texture2D& GetDepthTexture();

		FBO& GetGBuffer() {
			return gBuffer;
		}

		void ExportShadowMaps(const std::vector<OmniDirectionalShadowCaster*>& omniDirShadowCasters);

	private:
		FBO gBuffer;
		ShaderProgram fullScreenProgram;
		ShaderProgram fullScreenClearingProgram;
		ShaderProgram lightVolumeProgram;
		Texture2D gPosition;
		Texture2D gNormal;
		Texture2D gAlbedo;
		Texture2D gRoughnessMetallicAO;
		Texture2D depthTexture;
		Material* materialFullScreen = nullptr;
		Mesh* meshFullScreen = nullptr;
		Material* volumeMaterial = nullptr;
		Mesh* mesh = nullptr;
		std::vector<Geometry::LightInstancedData> instanceData;
	};
}

