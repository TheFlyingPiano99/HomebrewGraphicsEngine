#include "DeferredLightingSystem.h"
#include "MemoryManager.h"
#include "DirectionalShadowCaster.h"

namespace Hogra {
	
	void DeferredLightingSystem::Init(unsigned int _contextWidth, unsigned int _contextHeight) {
		gBuffer.Init();
		fullScreenProgram.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("fullScreenQuad.vert"),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("deferredPBRDirectionalLightingPass.frag"));
		
		fullScreenClearingProgram.Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("fullScreenQuad.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("deferredDarkPass.frag"));

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

		OnContextResize(_contextWidth, _contextHeight);
	}

	void DeferredLightingSystem::OnContextResize(unsigned int _contextWidth, unsigned int _contextHeight)
	{
		gPosition.Delete();
		gNormal.Delete();
		gAlbedo.Delete();
		gRoughnessMetallicAO.Delete();
		depthTexture.Delete();

		gPosition.Init(GL_RGBA32F, glm::ivec2(_contextWidth, _contextHeight), 0, GL_RGBA, GL_FLOAT);
		gNormal.Init(GL_RGBA16F, glm::ivec2(_contextWidth, _contextHeight), 1, GL_RGBA, GL_FLOAT);
		gAlbedo.Init(GL_RGBA16F, glm::ivec2(_contextWidth, _contextHeight), 2, GL_RGBA, GL_FLOAT);
		gRoughnessMetallicAO.Init(GL_RGBA8, glm::ivec2(_contextWidth, _contextHeight), 3, GL_RGBA, GL_UNSIGNED_BYTE);
		depthTexture.Init(GL_DEPTH_COMPONENT, glm::ivec2(_contextWidth, _contextHeight), 1, GL_DEPTH_COMPONENT, GL_FLOAT);

		gBuffer.LinkTexture(GL_COLOR_ATTACHMENT0, gPosition, 0);
		gBuffer.LinkTexture(GL_COLOR_ATTACHMENT1, gNormal, 0);
		gBuffer.LinkTexture(GL_COLOR_ATTACHMENT2, gAlbedo, 0);
		gBuffer.LinkTexture(GL_COLOR_ATTACHMENT3, gRoughnessMetallicAO, 0);
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
	
	void DeferredLightingSystem::Draw(const std::vector<PointLight*>& pointLights, const std::vector<DirectionalLight*>& dirLights) {
		meshFullScreen->Bind();
		fullScreenClearingProgram.Activate();
		meshFullScreen->Draw();					// Clear behind objects with black
		fullScreenProgram.Activate();

		// Directional lights:
		for (auto dirLight : dirLights) {
			if (dirLight->IsActive()) {
				if (dirLight->IsCastingShadow()) {
					dirLight->GetShadowCaster()->BindMap();
					fullScreenProgram.SetUniform("light.lightSpaceMatrix", dirLight->GetShadowCaster()->GetLightSpaceMatrix());
				}
				fullScreenProgram.SetUniform("light.direction", dirLight->GetDirection4D());
				fullScreenProgram.SetUniform("light.powerDensity", dirLight->getPowerDensity());
			}
			else {
				fullScreenProgram.SetUniform("light.direction", glm::vec4(0.0f));
				fullScreenProgram.SetUniform("light.powerDensity", glm::vec3(0.0f));
			}
			meshFullScreen->Draw();
		}

		// Point lights:
		mesh->Bind();
		instanceData.clear();
		for (int i = 0; i < pointLights.size(); i++) {
			if (pointLights[i]->IsActive()) {
				Geometry::LightInstancedData d = { 
					pointLights[i]->GetVolumeModelMatrix(),
					pointLights[i]->GetPosition4D(), 
					glm::vec4(pointLights[i]->getPowerDensity(), 0.0f),
					(nullptr != pointLights[i]->GetShadowCaster())? pointLights[i]->GetShadowCaster()->GetIdx() : -1
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
}