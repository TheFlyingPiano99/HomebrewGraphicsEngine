#include "DirectionalShadowCaster.h"

namespace Hogra {

	DirectionalShadowCaster::DirectionalShadowCaster() {
	}

	void DirectionalShadowCaster::Init(glm::vec3 _position, glm::vec3 _direction) {
		position = _position;
		direction = glm::normalize(_direction);

		fbo.Init();
		fbo.Bind();
		fbo.DisableDrawBuffer();
		fbo.DisableReadBuffer();
		shadowMap.Init(GL_DEPTH_COMPONENT, glm::ivec2(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT), SHADOW_MAP_UNIT, GL_DEPTH_COMPONENT, GL_FLOAT);
		shadowMap.Bind();

		// Configures the way the texture repeats (if it does at all)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		shadowMap.Unbind();
		fbo.LinkTexture(GL_DEPTH_ATTACHMENT, shadowMap);
		FBO::BindDefault();

		program.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("shadowCast.vert"),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("shadowCast.frag")
		);

		Update();
	}
	
	void DirectionalShadowCaster::Bind() {
		fbo.Bind();
		glClearDepth(1);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		program.Activate();
		program.SetUniform("lightSpaceMatrix", lightSpaceMatrix);
	}

	void DirectionalShadowCaster::BindMap() {
		shadowMap.Bind();
	}


	void DirectionalShadowCaster::Update() {
		if (nullptr != positionProvider) {
			position = positionProvider->GetPosition() + positionOffsetToProvider;
		}
		glm::mat4 lightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, DIR_SHADOW_MAP_NEAR_PLANE, DIR_SHADOW_MAP_FAR_PLANE);
		glm::mat4 lightView = glm::lookAt(
			position,
			position + direction,
			(0.99f > fabsf(glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), direction)))?glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(0.0f, 0.0f, 1.0f));
		lightSpaceMatrix = lightProjection * lightView;
	}

}
