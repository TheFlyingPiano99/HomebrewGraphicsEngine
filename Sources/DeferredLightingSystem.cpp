#include "DeferredLightingSystem.h"
#include "MemoryManager.h"
#include "DirectionalShadowCaster.h"
#include "DebugUtils.h"

namespace Hogra {
	
	static TextureCube emptyCubeMap;	// Only to avoid shader runtime errors when accessing unbound samplers.

	DeferredLightingSystem::~DeferredLightingSystem()
	{
		emptyCubeMap.Delete();
	}

	void DeferredLightingSystem::Init(unsigned int _contextWidth, unsigned int _contextHeight) {
		gBuffer.Init();
		fullScreenProgram.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("fullScreenQuad.vert"),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("deferredPBRDirectionalLightingPass.frag"));
		
		fullScreenSkyboxProgram.Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("fullScreenQuad.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("deferredAmbientPass.frag"));

		materialFullScreen = Allocator::New<Material>();
		materialFullScreen->Init(&fullScreenProgram);
		materialFullScreen->SetAlphaBlend(true);
		materialFullScreen->SetBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);	// Allow blending intensity of multiple directional light sources
		meshFullScreen = Allocator::New<Mesh>();
		meshFullScreen->Init(materialFullScreen, GeometryFactory::GetInstance()->GetSimpleQuad());
		meshFullScreen->SetDepthTest(false);
		meshFullScreen->setStencilTest(false);

		lightVolumeProgram.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("lightVolume.vert"),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("deferredPBRLightVolumePass.frag"));
		lightVolumeProgram.SetUniform("farPlane", OMNI_DIR_SHADOW_MAP_FAR_PLANE);
		volumeMaterial = Allocator::New<Material>();
		volumeMaterial->Init(&lightVolumeProgram);
		volumeMaterial->SetAlphaBlend(true);
		volumeMaterial->SetBlendFunc(GL_ONE, GL_ONE);
		mesh = Allocator::New<Mesh>();
		mesh->Init(volumeMaterial, GeometryFactory::GetInstance()->getLightVolumeSphere());
		mesh->SetDepthTest(false);
		mesh->setStencilTest(false);

		gBuffer.Bind();
		unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, attachments);

		emptyCubeMap.Init(8, SHADOW_MAP_UNIT, GL_DEPTH_COMPONENT, GL_FLOAT);

		OnContextResize(_contextWidth, _contextHeight);
	}

	void DeferredLightingSystem::OnContextResize(unsigned int _contextWidth, unsigned int _contextHeight)
	{
		gPosition.Delete();
		gNormal.Delete();
		gAlbedo.Delete();
		gRoughnessMetallicAO.Delete();
		depthTexture.Delete();

		gPosition.Init(GL_RGBA32F, glm::ivec2(_contextWidth, _contextHeight), POSITION_MAP_UNIT, GL_RGBA, GL_FLOAT);
		gAlbedo.Init(GL_RGBA16F, glm::ivec2(_contextWidth, _contextHeight), ALBEDO_MAP_UNIT, GL_RGBA, GL_FLOAT);
		gNormal.Init(GL_RGBA16F, glm::ivec2(_contextWidth, _contextHeight), NORMAL_MAP_UNIT, GL_RGBA, GL_FLOAT);
		gRoughnessMetallicAO.Init(GL_RGBA8, glm::ivec2(_contextWidth, _contextHeight), ROUGHNESS_METALLIC_AO_MAP_UNIT, GL_RGBA, GL_UNSIGNED_BYTE);
		depthTexture.Init(GL_DEPTH_COMPONENT, glm::ivec2(_contextWidth, _contextHeight), DEPTH_MAP_UNIT, GL_DEPTH_COMPONENT, GL_FLOAT);

		gBuffer.LinkTexture(GL_COLOR_ATTACHMENT0 + POSITION_MAP_UNIT, gPosition, 0);
		gBuffer.LinkTexture(GL_COLOR_ATTACHMENT0 + ALBEDO_MAP_UNIT, gAlbedo, 0);
		gBuffer.LinkTexture(GL_COLOR_ATTACHMENT0 + NORMAL_MAP_UNIT, gNormal, 0);
		gBuffer.LinkTexture(GL_COLOR_ATTACHMENT0 + ROUGHNESS_METALLIC_AO_MAP_UNIT, gRoughnessMetallicAO, 0);
		gBuffer.LinkTexture(GL_DEPTH_ATTACHMENT, depthTexture, 0);

		gBuffer.Unbind();

		volumeMaterial->ClearTextures();
		volumeMaterial->AddTexture(&gPosition);
		volumeMaterial->AddTexture(&gNormal);
		volumeMaterial->AddTexture(&gAlbedo);
		volumeMaterial->AddTexture(&gRoughnessMetallicAO);

		materialFullScreen->ClearTextures();
		materialFullScreen->AddTexture(&gPosition);
		materialFullScreen->AddTexture(&gNormal);
		materialFullScreen->AddTexture(&gAlbedo);
		materialFullScreen->AddTexture(&gRoughnessMetallicAO);
	}

	void DeferredLightingSystem::BindGBuffer() {
		gBuffer.Bind();
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glDepthFunc(GL_LESS);
		glFrontFace(GL_CCW);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	
	void DeferredLightingSystem::Draw(
		const std::vector<PointLight*>& pointLights, const std::vector<DirectionalLight*>& dirLights,
		TextureCube* environmentMap,
		TextureCube* irradianceMap,
		TextureCube* prefilterMap,
		Texture2D* brdfLUT
	) {
		meshFullScreen->Bind();
		fullScreenSkyboxProgram.Activate();
		if (nullptr != environmentMap) {
			environmentMap->Bind();
		}
		if (nullptr != irradianceMap) {
			irradianceMap->Bind();
		}
		if (nullptr != prefilterMap) {
			prefilterMap->Bind();
		}
		if (nullptr != brdfLUT) {
			brdfLUT->Bind();
		}
		meshFullScreen->Draw();					// Draw skybox reflections
		fullScreenProgram.Activate();

		// Directional lights:
		for (auto dirLight : dirLights) {
			if (dirLight->IsActive()) {
				if (dirLight->IsCastingShadow()) {
					dirLight->GetShadowCaster()->BindMap();
					fullScreenProgram.SetUniform("light.lightSpaceMatrix", dirLight->GetShadowCaster()->GetLightSpaceMatrix());
				}
				dirLight->GetShadowCaster()->BindMap();
				fullScreenProgram.SetUniform("light.direction", dirLight->GetDirection4D());
				fullScreenProgram.SetUniform("light.powerDensity", dirLight->getPowerDensity());
				meshFullScreen->Draw();
			}
		}

		// Point lights:
		mesh->Bind();
		instanceData.clear();
		emptyCubeMap.Bind();
		volumeMaterial->GetShaderProgram()->SetUniform("shadowMaps[0]", &emptyCubeMap);	// First in array is the default, empty map
		for (int i = 0; i < pointLights.size(); i++) {
			if (pointLights[i]->IsActive()) {
				int casterIdx = 0;
				if (nullptr != pointLights[i]->GetShadowCaster()) {
					auto caster = pointLights[i]->GetShadowCaster();
					casterIdx = caster->GetIdx();
					caster->GetShadowMap()->Bind();
					volumeMaterial->GetShaderProgram()->SetUniform(std::string("shadowMaps[").append(std::to_string(casterIdx)).append("]").c_str(), caster->GetShadowMap());
				}
				Geometry::LightInstancedData d = {
					pointLights[i]->GetVolumeModelMatrix(),
					pointLights[i]->GetPosition4D(), 
					glm::vec4(pointLights[i]->getPowerDensity(), 0.0f),
					(float)casterIdx
				};
				instanceData.push_back(d);
			}
		}
		if (!instanceData.empty()) {
			mesh->DrawInstanced(instanceData);
		}
	}

	const Texture2D& DeferredLightingSystem::GetDepthTexture()
	{
		return depthTexture;
	}
	
	void DeferredLightingSystem::ExportShadowMaps(const std::vector<OmniDirectionalShadowCaster*>& omniDirShadowCasters)
	{
		volumeMaterial->GetShaderProgram()->Activate();
		unsigned int i = 0;
		volumeMaterial->GetShaderProgram()->SetUniform("shadowMaps[0]", &emptyCubeMap);	// First in array is the default, empty map
		for (; i < omniDirShadowCasters.size(); i++) {
			volumeMaterial->GetShaderProgram()->SetUniform(std::string("shadowMaps[").append(std::to_string(i + 1)).append("]").c_str(), omniDirShadowCasters[i]->GetShadowMap());
		}
		for (; i < MAX_SHADOW_MAP_COUNT; i++) {	// Fill the remaining samplers in the array with default, empty map
			volumeMaterial->GetShaderProgram()->SetUniform(std::string("shadowMaps[").append(std::to_string(i + 1)).append("]").c_str(), &emptyCubeMap);
		}
		volumeMaterial->GetShaderProgram()->SetUniform("farPlane", OMNI_DIR_SHADOW_MAP_FAR_PLANE);
	}
}