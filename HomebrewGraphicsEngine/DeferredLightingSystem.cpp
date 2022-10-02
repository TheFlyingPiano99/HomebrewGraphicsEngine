#include "DeferredLightingSystem.h"
#include "MemoryManager.h"

namespace Hogra {
	
	void DeferredLightingSystem::Init(int width, int height) {
		gBuffer.Init();
		fullScreenProgram.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("fullscreenQuad.vert"),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("deferredPBRshading.frag"));
		materialFullScreen = Allocator<Material>::New();
		materialFullScreen->Init(&fullScreenProgram);
		meshFullScreen = Allocator<Mesh>::New();
		meshFullScreen->Init(materialFullScreen, GeometryFactory::GetInstance()->getFullScreenQuad());
		meshFullScreen->setDepthTest(false);
		meshFullScreen->setStencilTest(false);

		lightVolumeProgram.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("lightVolume.vert"),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("deferredPBRshadingLightVolume.frag"));
		material = Allocator<Material>::New();
		material->Init(&lightVolumeProgram);
		material->setAlphaBlend(true);
		material->setBlendFunc(GL_ONE, GL_ONE);
		mesh = Allocator<Mesh>::New();
		mesh->Init(material, GeometryFactory::GetInstance()->getLightVolumeSphere());
		mesh->setDepthTest(false);
		mesh->setStencilTest(false);
		Resize(width, height);
	}
	void DeferredLightingSystem::Resize(int width, int height) {
		gPosition.Delete();
		gNormal.Delete();
		gAlbedo.Delete();
		gRoughnessMetallicAO.Delete();
		depthTexture.Delete();

		gPosition.Init(GL_RGBA32F, glm::ivec2(width, height), 0, GL_RGBA, GL_FLOAT);
		gNormal.Init(GL_RGBA16F, glm::ivec2(width, height), 1, GL_RGBA, GL_FLOAT);
		gAlbedo.Init(GL_RGBA16F, glm::ivec2(width, height), 2, GL_RGBA, GL_FLOAT);
		gRoughnessMetallicAO.Init(GL_RGBA8, glm::ivec2(width, height), 3, GL_RGBA, GL_UNSIGNED_BYTE);
		depthTexture.Init(GL_DEPTH_COMPONENT, glm::ivec2(width, height), 1, GL_DEPTH_COMPONENT, GL_FLOAT);
		gBuffer.LinkTexture(GL_COLOR_ATTACHMENT0, gPosition, 0);
		gBuffer.LinkTexture(GL_COLOR_ATTACHMENT1, gNormal, 0);
		gBuffer.LinkTexture(GL_COLOR_ATTACHMENT2, gAlbedo, 0);
		gBuffer.LinkTexture(GL_COLOR_ATTACHMENT3, gRoughnessMetallicAO, 0);
		gBuffer.LinkTexture(GL_DEPTH_ATTACHMENT, depthTexture, 0);
		unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, attachments);
		material->clearTextures();
		material->addTexture(&gPosition);
		material->addTexture(&gNormal);
		material->addTexture(&gAlbedo);
		material->addTexture(&gRoughnessMetallicAO);
		materialFullScreen->clearTextures();
		materialFullScreen->addTexture(&gPosition);
		materialFullScreen->addTexture(&gNormal);
		materialFullScreen->addTexture(&gAlbedo);
		materialFullScreen->addTexture(&gRoughnessMetallicAO);
		gBuffer.Unbind();
	}
	void DeferredLightingSystem::BindGBuffer() {
		gBuffer.Bind();
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1);
		glDepthFunc(GL_LESS);
		glFrontFace(GL_CCW);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glStencilMask(0x00);
	}
	
	void DeferredLightingSystem::Draw(const std::vector<Light*>& pointLights, const Light& directionalLight) {
		meshFullScreen->Bind();
		glm::vec4 pos = directionalLight.GetPosition4D();
		glm::vec3 pow = directionalLight.getPowerDensity();
		if (directionalLight.IsCastShadow()) {
			glUniform4f(glGetUniformLocation(fullScreenProgram.ID, "light.position"), pos.x, pos.y, pos.z, pos.w);
			glUniform3f(glGetUniformLocation(fullScreenProgram.ID, "light.powerDensity"), pow.x, pow.y, pow.z);
		}
		else {
			glUniform4f(glGetUniformLocation(fullScreenProgram.ID, "light.position"), 0, 0, 0, 0);
			glUniform3f(glGetUniformLocation(fullScreenProgram.ID, "light.powerDensity"), 0, 0, 0);
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
			mesh->DrawInstanced(instanceData);
		}
	}

	const Texture2D& DeferredLightingSystem::GetDepthTexture()
	{
		return depthTexture;
	}
}