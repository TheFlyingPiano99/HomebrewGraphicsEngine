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
			
		~DeferredLightingSystem() {
		};

		void Init(int width, int height) {
			gBuffer.Init();
			fullScreenProgram.Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("fullscreenQuad.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("deferredPBRshading.frag"));
			materialFullScreen = Material::Instantiate();
			materialFullScreen->Init(&fullScreenProgram);
			meshFullScreen = Mesh::Instantiate();
			meshFullScreen->Init(materialFullScreen, GeometryFactory::GetInstance()->getFullScreenQuad());
			meshFullScreen->setDepthTest(false);
			meshFullScreen->setStencilTest(false);

			lightVolumeProgram.Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("lightVolume.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("deferredPBRshadingLightVolume.frag"));
			material = Material::Instantiate();
			material->Init(&lightVolumeProgram);
			material->setAlphaBlend(true);
			material->setBlendFunc(GL_ONE, GL_ONE);
			mesh = Mesh::Instantiate();
			mesh->Init(material, GeometryFactory::GetInstance()->getLightVolumeSphere());
			mesh->setDepthTest(false);
			mesh->setStencilTest(false);
			Resize(width, height);
		}

		void Resize(int width, int height) {
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

		void BindGBuffer() {
			gBuffer.Bind();
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClearDepth(1);
			glDepthFunc(GL_LESS);
			glFrontFace(GL_CCW);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glStencilMask(0x00);
		}

		void Draw(const std::vector<Light*>& lights) {
			meshFullScreen->Bind();
			glm::vec4 pos = lights[0]->GetPosition();
			glm::vec3 pow = lights[0]->getPowerDensity();
			glUniform4f(glGetUniformLocation(fullScreenProgram.ID, "light.position"), pos.x , pos.y, pos.z, pos.w);
			glUniform3f(glGetUniformLocation(fullScreenProgram.ID, "light.powerDensity"), pow.x, pow.y, pow.z);
			meshFullScreen->Draw();

			mesh->Bind();
			instanceData.clear();
			for (int i = 1; i < lights.size(); i++) {
				Geometry::LightInstancedData d = { lights[i]->getVolumeModelMatrix(), lights[i]->GetPosition(), glm::vec4(lights[i]->getPowerDensity(), 0.0)};
				instanceData.push_back(d);
			}
			mesh->DrawInstanced(instanceData);
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

