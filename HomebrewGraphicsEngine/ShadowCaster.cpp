#include "ShadowCaster.h"
namespace hograengine {
	ShadowCaster::ShadowCaster(glm::vec3 position, glm::vec3 direction) : position(position), direction(glm::normalize(direction)) {
		fbo.Bind();
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		shadowMap = new Texture2D(GL_DEPTH_COMPONENT, glm::ivec2(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT), SHADOW_MAP_UNIT, GL_DEPTH_COMPONENT, GL_FLOAT);
		shadowMap->Bind();
		// Configures the way the texture repeats (if it does at all)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		shadowMap->Unbind();
		program = new ShaderProgram(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("shadowCast.vert"),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("shadowCast.frag")
		);
		fbo.LinkTexture(GL_DEPTH_ATTACHMENT, *shadowMap);
		FBO::BindDefault();
		update();

		std::vector<int> dataSizes;
		// lightSpaceMatrix:
		dataSizes.push_back(sizeof(glm::vec4));
		dataSizes.push_back(sizeof(glm::vec4));
		dataSizes.push_back(sizeof(glm::vec4));
		dataSizes.push_back(sizeof(glm::vec4));
		ubo = new UniformBufferObject(dataSizes, SHADOWCASTER_UBO_BINDING);
	}
}
