#include "OmniDirectionalShadowCaster.h"

#include<glm/gtc/matrix_transform.hpp>
#include "GlobalInclude.h"
#include "AssetFolderPathManager.h"

namespace Hogra {

	void OmniDirectionalShadowCaster::Init(const glm::vec3& pos)
	{
		static unsigned int nextUnit = 14;
		shadowMap.Init(1024, nextUnit++, GL_DEPTH_COMPONENT, GL_FLOAT);
		fbo.Init();
		fbo.LinkTexture(GL_DEPTH_ATTACHMENT, shadowMap);
		fbo.DisableDrawBuffer();
		fbo.DisableReadBuffer();

		program.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("omniDirShadowCast.vert"),
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("omniDirShadowCast.geom"),
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("omniDirShadowCast.frag")
			);
	}

	void OmniDirectionalShadowCaster::Update()
	{
		if (nullptr != positionProvider) {
			position = positionProvider->GetPosition();
		}
	
		float aspect = (float)shadowMap.GetDimensions().x / (float)shadowMap.GetDimensions().y;
		shadowProj = glm::perspective(glm::radians(90.0f), aspect, OMNI_DIR_SHADOW_MAP_NEAR_PLANE, OMNI_DIR_SHADOW_MAP_FAR_PLANE);

		shadowTransforms[0] = shadowProj *
			glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0) * OMNI_DIR_SHADOW_MAP_NEAR_PLANE, glm::vec3(0.0, -1.0, 0.0));
		shadowTransforms[1] = shadowProj *
			glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0) * OMNI_DIR_SHADOW_MAP_NEAR_PLANE, glm::vec3(0.0, -1.0, 0.0));
		shadowTransforms[2] = shadowProj *
			glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0) * OMNI_DIR_SHADOW_MAP_NEAR_PLANE, glm::vec3(0.0, 0.0, 1.0));
		shadowTransforms[3] = shadowProj *
			glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0) * OMNI_DIR_SHADOW_MAP_NEAR_PLANE, glm::vec3(0.0, 0.0, -1.0));
		shadowTransforms[4] = shadowProj *
			glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0) * OMNI_DIR_SHADOW_MAP_NEAR_PLANE, glm::vec3(0.0, -1.0, 0.0));
		shadowTransforms[5] = shadowProj *
			glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0) * OMNI_DIR_SHADOW_MAP_NEAR_PLANE, glm::vec3(0.0, -1.0, 0.0));

	}

	void OmniDirectionalShadowCaster::Bind() {
		fbo.Bind();
		glClearDepth(1);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		program.Activate();
		program.SetUniform("shadowTransforms[0]", shadowTransforms[0]);
		program.SetUniform("shadowTransforms[1]", shadowTransforms[1]);
		program.SetUniform("shadowTransforms[2]", shadowTransforms[2]);
		program.SetUniform("shadowTransforms[3]", shadowTransforms[3]);
		program.SetUniform("shadowTransforms[4]", shadowTransforms[4]);
		program.SetUniform("shadowTransforms[5]", shadowTransforms[5]);
		program.SetUniform("casterPosition", position);
		program.SetUniform("farPlane", OMNI_DIR_SHADOW_MAP_FAR_PLANE);	// Set on Init
	}

}