#include "chunk.h"
#include "../AssetFolderPathManager.h"
#include "../GeometryFactory.h"


namespace Hogra::FallingSand {
	
	void Chunk::Init(Mesh* _mesh)
	{
		mesh = _mesh;
		texture.Init(std::vector<glm::vec4>(grid.GetSize()), grid.GetDimensions(), 0, GL_RGBA, GL_FLOAT);
		mesh->getMaterial()->AddTexture(&texture);
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

}
