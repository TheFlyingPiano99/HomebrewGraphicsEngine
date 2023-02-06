#include "DeferredLightingSystem.h"
#include "MemoryManager.h"

namespace Hogra {
	
	void DeferredLightingSystem::Init(unsigned int _contextWidth, unsigned int _contextHeight) {
		gBuffer.Init();
		fullScreenProgram.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("quad.vert"),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("deferredPBRshadingDirectionalLight.frag"));
		materialFullScreen = Allocator::New<Material>();
		materialFullScreen->Init(&fullScreenProgram);
		meshFullScreen = Allocator::New<Mesh>();
		meshFullScreen->Init(materialFullScreen, GeometryFactory::GetInstance()->GetSimpleQuad());
		meshFullScreen->SetDepthTest(false);
		meshFullScreen->setStencilTest(false);

		lightVolumeProgram.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("lightVolume.vert"),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("deferredPBRshadingLightVolume.frag"));
		material = Allocator::New<Material>();
		material->Init(&lightVolumeProgram);
		material->SetAlphaBlend(true);
		material->SetBlendFunc(GL_ONE, GL_ONE);
		mesh = Allocator::New<Mesh>();
		mesh->Init(material, GeometryFactory::GetInstance()->getLightVolumeSphere());
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

		material->ClearTextures();
		material->AddTexture(&gPosition);
		material->AddTexture(&gNormal);
		material->AddTexture(&gAlbedo);
		material->AddTexture(&gRoughnessMetallicAO);

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
	
	void DeferredLightingSystem::Draw(const std::vector<Light*>& pointLights, const Light& directionalLight) {
		meshFullScreen->Bind();
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glm::vec4 pos = directionalLight.GetPosition4D();
		glm::vec3 pow = directionalLight.getPowerDensity();
		if (directionalLight.IsCastShadow()) {
			fullScreenProgram.SetUniform("light.position", pos);
			fullScreenProgram.SetUniform("light.powerDensity", pow);
		}
		else {
			fullScreenProgram.SetUniform("light.position", glm::vec4(0.0f));
			fullScreenProgram.SetUniform("light.powerDensity", glm::vec3(0.0f));
		}
		meshFullScreen->Draw();

		if (1 < pointLights.size()) {
			mesh->Bind();
			instanceData.clear();
			for (int i = 1; i < pointLights.size(); i++) {
				if (pointLights[i]->IsActive() && pointLights[i]->IsCastShadow()) {
					Geometry::LightInstancedData d = { pointLights[i]->getVolumeModelMatrix(), pointLights[i]->GetPosition4D(), glm::vec4(pointLights[i]->getPowerDensity(), 0.0) };
					instanceData.push_back(d);
				}
			}
			if (!instanceData.empty()) {
				mesh->DrawInstanced(instanceData);
			}
		}
	}

	const Texture2D& DeferredLightingSystem::GetDepthTexture()
	{
		return depthTexture;
	}
}