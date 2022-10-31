#pragma once
#include "particle.h"

namespace Hogra::FallingSand {

	class Liquid : public Particle {
	public:
		Liquid() {
			type = Type::LiquidParticle;
		}

		void Update(ParticleGrid& grid, unsigned int x, unsigned int y, float dt);

	private:

	};
}