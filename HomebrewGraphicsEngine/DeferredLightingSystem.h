#pragma once
#include "FBO.h"
#include "AssetFolderPathManager.h"
#include "Material.h"
#include "Mesh.h"
#include "GeometryFactory.h"

namespace hograengine {
	class DeferredLightingSystem
	{
	public:
		DeferredLightingSystem() = default;
			
		~DeferredLightingSystem() {
			if (gPosition != nullptr) {
				delete gPosition;
				delete gNormal;
				delete gAlbedo;
				delete gRoughnessMetallicAO;
				delete depthTexture;
			}
			if (program != nullptr) {
				delete program;
				delete material;
				delete mesh;
			}
		};

		void Init(int width, int height) {
			program = new ShaderProgram(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("fullscreenQuad.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("deferredPBRshading.frag"));
			material = new Material(program);
			mesh = new Mesh(material, GeometryFactory::FullScreenQuad::getInstance());
			mesh->setDepthTest(false);
			mesh->setStencilTest(false);
			Resize(width, height);
		}

		void Resize(int width, int height) {
			if (gPosition != nullptr) {
				delete gPosition;
				delete gNormal;
				delete gAlbedo;
				delete gRoughnessMetallicAO;
				delete depthTexture;
			}
			
			gPosition = new Texture2D(GL_RGBA32F, glm::ivec2(width, height), 0, GL_RGBA, GL_FLOAT);
			gNormal = new Texture2D(GL_RGBA16F, glm::ivec2(width, height), 1, GL_RGBA, GL_FLOAT);
			gAlbedo = new Texture2D(GL_RGBA8, glm::ivec2(width, height), 2, GL_RGBA, GL_UNSIGNED_BYTE);
			gRoughnessMetallicAO = new Texture2D(GL_RGBA8, glm::ivec2(width, height), 3, GL_RGBA, GL_UNSIGNED_BYTE);
			depthTexture = new Texture2D(GL_DEPTH_COMPONENT, glm::ivec2(width, height), 1, GL_DEPTH_COMPONENT, GL_FLOAT);
			gBuffer.LinkTexture(GL_COLOR_ATTACHMENT0, *gPosition, 0);
			gBuffer.LinkTexture(GL_COLOR_ATTACHMENT1, *gNormal, 0);
			gBuffer.LinkTexture(GL_COLOR_ATTACHMENT2, *gAlbedo, 0);
			gBuffer.LinkTexture(GL_COLOR_ATTACHMENT3, *gRoughnessMetallicAO, 0);
			gBuffer.LinkTexture(GL_DEPTH_ATTACHMENT, *depthTexture, 0);
			unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(4, attachments);
			material->clearTextures();
			material->addTexture(gPosition);
			material->addTexture(gNormal);
			material->addTexture(gAlbedo);
			material->addTexture(gRoughnessMetallicAO);
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

		void Draw() {
			mesh->Bind();
			mesh->Draw();
		}

	private:
		FBO gBuffer;
		ShaderProgram* program = nullptr;
		Texture2D* gPosition = nullptr;
		Texture2D* gNormal = nullptr;
		Texture2D* gAlbedo = nullptr;
		Texture2D* gRoughnessMetallicAO = nullptr;
		Texture2D* depthTexture = nullptr;
		Material* material = nullptr;
		Mesh* mesh = nullptr;

	};
}

