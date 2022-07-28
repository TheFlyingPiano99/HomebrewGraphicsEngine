#pragma once

#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include "ShaderProgram.h"
#include "FBO.h"
#include "Camera.h"
#include "ShaderProgram.h"
#include "GlobalInclude.h"
#include "AssetFolderPathManager.h"

namespace Hogra {
	class ShadowCaster
	{
	public:
		ShadowCaster(glm::vec3 position, glm::vec3 direction);

		~ShadowCaster() {
		}

		const Texture2D* getShadowMap() const {
			return shadowMap;
		}

		void Bind() {
			fbo.Bind();
			glClearDepth(1);
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			program.Activate();
			ExportData();
		}

		void Update() {
			if (nullptr != positionProvider) {
				position = positionProvider->GetPosition() + positionOffsetToProvider;
			}
			float near_plane = 1.0f, far_plane = 100.0f;
			glm::mat4 lightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, near_plane, far_plane);
			glm::mat4 lightView = glm::lookAt(position,
				position + direction,
				glm::vec3(0.0f, 1.0f, 0.0f));
			lightSpaceMatrix = lightProjection * lightView;
		}

		const ShaderProgram& getProgram() const {
			return program;
		}

		void ExportData() {
			ubo.Bind();
			ubo.UploadSubData((void*)glm::value_ptr(lightSpaceMatrix[0]), 0);
			ubo.UploadSubData((void*)glm::value_ptr(lightSpaceMatrix[1]), 1);
			ubo.UploadSubData((void*)glm::value_ptr(lightSpaceMatrix[2]), 2);
			ubo.UploadSubData((void*)glm::value_ptr(lightSpaceMatrix[3]), 3);
			ubo.Unbind();
			shadowMap->Bind();
		}

		void SetPositionProvider(PositionProvider* provider) {
			positionProvider = provider;
		}

		void setPositionOffsetToProvider(const glm::vec3& offset) {
			positionOffsetToProvider = offset;
		}

	private:
		glm::vec3 position;
		glm::vec3 direction;

		FBO fbo;
		UniformBufferObject ubo;
		Texture2D* shadowMap = nullptr;	// Do not delete in this object
		ShaderProgram program;
		glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
		PositionProvider* positionProvider = nullptr;
		glm::vec3 positionOffsetToProvider = glm::vec3(0.0f);
	};
}
