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

		Texture2D& getShadowMap() {
			return shadowMap;
		}

		void Bind();

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

		void ExportData();

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
		Texture2D shadowMap;
		ShaderProgram program;
		glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
		PositionProvider* positionProvider = nullptr;
		glm::vec3 positionOffsetToProvider = glm::vec3(0.0f);
	};
}
