#pragma once
#include <vector>
#include "particle.h"
#include "../FBO.h"
#include "../Component.h"
#include "../Texture2D.h"
#include "../Geometry.h"

namespace Hogra::FallingSand {

	class Chunk : public Component {
	public:

		Chunk();

		void LatePhysicsUpdate(float dt) override;

		void AfterPhysicsLoopUpdate() override;

		void Draw(FBO& outFBO, const Texture2D& depthTexture, const Camera& camera) override;

		ParticleGrid& GetGrid() {
			return grid;
		}

	private:
		ParticleGrid grid;
		Texture2D texture;
		ShaderProgram program;
		Geometry* fullScreenQuad = nullptr;
	};
}