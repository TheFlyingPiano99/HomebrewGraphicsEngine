#pragma once
#include "FBO.h"
#include "AssetFolderPathManager.h"
#include "Material.h"
#include "Mesh.h"
#include "GeometryFactory.h"
#include "glm/gtx/transform.hpp"

namespace Hogra {
	class DeferredLightingSystem
	{
	public:
		DeferredLightingSystem() = default;
			
		~DeferredLightingSystem() = default;

		void Init(int width, int height);

		void Resize(int width, int height);

		void BindGBuffer();

		void Draw(const std::vector<Light*>& pointLights, const Light& directionalLight);

		const Texture2D& GetDepthTexture();

		FBO& GetGBuffer() {
			return gBuffer;
		}

	private:
		FBO gBuffer;
		ShaderProgram fullScreenProgram;
		ShaderProgram lightVolumeProgram;
		Texture2D gPosition;
		Texture2D gNormal;
		Texture2D gAlbedo;
		Texture2D gRoughnessMetallicAO;
		Texture2D depthTexture;
		Material* materialFullScreen = nullptr;
		Mesh* meshFullScreen = nullptr;
		Material* material = nullptr;
		Mesh* mesh = nullptr;
		std::vector<Geometry::LightInstancedData> instanceData;
	};
}

