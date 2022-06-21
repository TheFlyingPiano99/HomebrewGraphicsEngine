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
		ShadowCaster(glm::vec3 position, glm::vec3 direction) : position(position), direction(glm::normalize(direction)) {
			fbo.Bind();
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			shadowMap = new Texture2D(GL_DEPTH_COMPONENT, glm::ivec2(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT), SHADOW_MAP_UNIT, GL_DEPTH_COMPONENT, GL_FLOAT);
			shadowMap->Bind();
			// Configures the way the texture repeats (if it does at all)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
			shadowMap->Unbind();
			program = new ShaderProgram(
				AssetFolderPath::getInstance()->getShaderFolderPath().append("shadowCast.vert"),
				AssetFolderPath::getInstance()->getShaderFolderPath().append("shadowCast.frag")
			);
			fbo.LinkTexture(GL_DEPTH_ATTACHMENT, *shadowMap);
			FBO::BindDefault();
			update();
		}

		~ShadowCaster() {
			if (program != nullptr) {
				delete program;
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
			exportData(*program);
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

		void exportData(const ShaderProgram& _program) const {
			glUniformMatrix4fv(glGetUniformLocation(_program.ID, "shadowCaster.lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
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
		Texture2D* shadowMap = nullptr;	// Do not delete in this object
		ShaderProgram* program = nullptr;
		glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
		PositionProvider* positionProvider = nullptr;
		glm::vec3 positionOffsetToProvider = glm::vec3(0.0f);
	};
}
