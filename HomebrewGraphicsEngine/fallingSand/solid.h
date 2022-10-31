#pragma once
#include "particle.h"

namespace Hogra::FallingSand {

	class Solid : public Particle {
	public:
		Solid() {
			type = Type::SolidParticle;
		}

		void Update(ParticleGrid& grid, unsigned int x, unsigned int y, float dt) override;

	private:

	};
}