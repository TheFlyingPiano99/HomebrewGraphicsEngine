#pragma once
#include "particle.h"

namespace Hogra::FallingSand {

	class Solid : public Particle {
	public:
		Solid() {
			type = Type::SolidParticle;
		}

		bool Update(ParticleGrid& grid, unsigned int x, unsigned int y, float dt) override;

	protected:
		float meltingTemperature = 100.0f;
		std::function<Particle*()> liquidConstructor;
	};
}