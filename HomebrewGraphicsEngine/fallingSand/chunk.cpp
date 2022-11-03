#include "chunk.h"
#include "../AssetFolderPathManager.h"
#include "../GeometryFactory.h"


namespace Hogra::FallingSand {
	Chunk::Chunk()
	{
		fullScreenQuad = GeometryFactory::GetInstance()->getFullScreenQuad();
		texture.Init(std::vector<glm::vec4>(grid.GetSize()), grid.GetDimensions(), 0, GL_RGBA, GL_FLOAT);
		program.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("fullscreenQuad.vert"),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("bypass.frag")
			);

	}

	void Chunk::LatePhysicsUpdate(float dt)
	{
		grid.Update(dt);
	}

	void Chunk::AfterPhysicsLoopUpdate()
	{
		grid.UpdateColorData();
		texture.SetData(grid.GetColorData());
	}

	void Chunk::Draw(FBO& outFBO, const Texture2D& depthTexture, const Camera& camera)
	{
		outFBO.Bind();
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		program.Activate();
		texture.Bind();
		fullScreenQuad->BindVAO();
		fullScreenQuad->Draw();
		outFBO.Unbind();
	}

}
