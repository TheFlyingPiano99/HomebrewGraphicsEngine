#pragma once
#include <vector>
#include "particle.h"
#include "../FBO.h"
#include "../Component.h"
#include "../Texture2D.h"
#include "../Mesh.h"

namespace Hogra::FallingSand {

	class Chunk : public Component {
	public:

		void Init(Mesh* _mesh);

		void LatePhysicsUpdate(float dt) override;

		void AfterPhysicsLoopUpdate() override;

		ParticleGrid& GetGrid() {
			return grid;
		}

	private:
		ParticleGrid grid;
		Texture2D texture;
		ShaderProgram program;
		Mesh* mesh = nullptr;
	};
}