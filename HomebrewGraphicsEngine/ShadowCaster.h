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

		ShadowCaster();

		void Init(glm::vec3 _position, glm::vec3 _direction) {
			position = _position;
			direction = glm::normalize(_direction);
			fbo.Init();
			fbo.Bind();
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			shadowMap.Init(GL_DEPTH_COMPONENT, glm::ivec2(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT), SHADOW_MAP_UNIT, GL_DEPTH_COMPONENT, GL_FLOAT);
			shadowMap.Bind();
			// Configures the way the texture repeats (if it does at all)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
			shadowMap.Unbind();
			program.Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("shadowCast.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("shadowCast.frag")
			);
			fbo.LinkTexture(GL_DEPTH_ATTACHMENT, shadowMap);
			FBO::BindDefault();
			Update();

			std::vector<int> dataSizes;
			// lightSpaceMatrix:
			dataSizes.push_back(sizeof(glm::vec4));
			dataSizes.push_back(sizeof(glm::vec4));
			dataSizes.push_back(sizeof(glm::vec4));
			dataSizes.push_back(sizeof(glm::vec4));
			ubo.Init(dataSizes, SHADOWCASTER_UBO_BINDING);

		}

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
