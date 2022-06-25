#pragma once

#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include "ShaderProgram.h"
#include "FBO.h"
#include "Camera.h"
#include "ShaderProgram.h"
#include "GlobalInclude.h"
#include "AssetFolderPathManager.h"

namespace hograengine {
	class ShadowCaster
	{
	public:
		ShadowCaster(glm::vec3 position, glm::vec3 direction);

		~ShadowCaster() {
			if (program != nullptr) {
				delete program;
			}
			if (ubo != nullptr) {
				delete ubo;
			}
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
			program->Activate();
			exportData();
		}

		void update() {
			if (nullptr != positionProvider) {
				position = positionProvider->getPosition() + positionOffsetToProvider;
			}
			float near_plane = 1.0f, far_plane = 500.0f;
			glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
			glm::mat4 lightView = glm::lookAt(position,
				position + direction,
				glm::vec3(0.0f, 1.0f, 0.0f));
			lightSpaceMatrix = lightProjection * lightView;
		}

		const ShaderProgram& getProgram() const {
			return *program;
		}

		void exportData() const {
			ubo->Bind();
			ubo->uploadSubData((void*)glm::value_ptr(lightSpaceMatrix[0]), 0);
			ubo->uploadSubData((void*)glm::value_ptr(lightSpaceMatrix[1]), 1);
			ubo->uploadSubData((void*)glm::value_ptr(lightSpaceMatrix[2]), 2);
			ubo->uploadSubData((void*)glm::value_ptr(lightSpaceMatrix[3]), 3);
			ubo->Unbind();
			shadowMap->Bind();
		}

		void setPositionProvider(PositionProvider* provider) {
			positionProvider = provider;
		}

		void setPositionOffsetToProvider(const glm::vec3& offset) {
			positionOffsetToProvider = offset;
		}

	private:
		glm::vec3 position;
		glm::vec3 direction;

		FBO fbo;
		UniformBufferObject* ubo = nullptr;
		Texture2D* shadowMap = nullptr;	// Do not delete in this object
		ShaderProgram* program = nullptr;
		glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
		PositionProvider* positionProvider = nullptr;
		glm::vec3 positionOffsetToProvider = glm::vec3(0.0f);
	};
}
