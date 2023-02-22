#pragma once

#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include "ShaderProgram.h"
#include "FBO.h"
#include "Camera.h"
#include "ShaderProgram.h"
#include "GlobalInclude.h"
#include "AssetFolderPathManager.h"
#include "ShadowCaster.h"

namespace Hogra {
	class DirectionalShadowCaster : public ShadowCaster
	{
	public:

		DirectionalShadowCaster();

		void Init(glm::vec3 _position = glm::vec3(), glm::vec3 _direction = glm::vec3());

		~DirectionalShadowCaster() {
		}

		Texture2D& GetShadowMap() {
			return shadowMap;
		}

		void Bind();

		void BindMap();

		void Update() override;

		const ShaderProgram& GetProgram() const {
			return program;
		}

		void SetPositionProvider(PositionProvider* provider) {
			positionProvider = provider;
		}

		void SetPositionOffsetToProvider(const glm::vec3& offset) {
			positionOffsetToProvider = offset;
		}

		const glm::mat4& GetLightSpaceMatrix() {
			return lightSpaceMatrix;
		}

		void SetDirection(const glm::vec3& dir) {
			direction = dir;
		}

	private:
		glm::vec3 position;
		glm::vec3 direction;

		FBO fbo;
		Texture2D shadowMap;
		ShaderProgram program;
		glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
		PositionProvider* positionProvider = nullptr;
		glm::vec3 positionOffsetToProvider = glm::vec3(0.0f);
	};
}
