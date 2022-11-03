#pragma once
#include "particle.h"

namespace Hogra::FallingSand {

	class Gas : public Particle {
	public:
		Gas() {
			type = Type::GasParticle;
		}

		bool Update(ParticleGrid& grid, unsigned int x, unsigned int y, float dt);

	protected:
		float condensationTemperature = 0.0f;
		std::function<Particle* ()> liquidConstructor;
	};
}