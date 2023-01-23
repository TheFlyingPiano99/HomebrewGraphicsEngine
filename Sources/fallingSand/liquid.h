#pragma once
#include "particle.h"

namespace Hogra::FallingSand {

	class Liquid : public Particle {
	public:
		Liquid() {
			type = Type::LiquidParticle;
		}

		bool Update(ParticleGrid& grid, unsigned int x, unsigned int y, float dt);

	protected:
		float boilingTemperature = 100.0f;
		float freezingTemperature = 0.0f;
		std::function<Particle* ()> gasConstructor;
		std::function<Particle* ()> solidConstructor;
	};
}