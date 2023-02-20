#pragma once
#include<glm/glm.hpp>
#include "ShadowCaster.h"
#include "TextureCube.h"
#include "FBO.h"
#include "PositionProvider.h"


namespace Hogra {
	class OmniDirectionalShadowCaster : public ShadowCaster
	{
	public:

		void Init(const glm::vec3& pos = glm::vec3(0.0f, 0.0f, 0.0f));

		void Bind();

		void Update() override;

		void SetPositionProvider(PositionProvider* provider) {
			positionProvider = provider;
		}

		const TextureCube* GetShadowMap() const {
			return &shadowMap;
		}

	private:
		TextureCube shadowMap;
		FBO fbo;
		ShaderProgram program;

		glm::vec3 position;
		glm::mat4 shadowProj;
		glm::mat4 shadowTransforms[6];
		PositionProvider* positionProvider;

	};
}

