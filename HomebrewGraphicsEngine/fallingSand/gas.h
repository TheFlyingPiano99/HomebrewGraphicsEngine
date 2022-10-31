#pragma once
#include "particle.h"

namespace Hogra::FallingSand {

	class Gas : public Particle {
	public:
		Gas() {
			type = Type::GasParticle;
		}

	private:

	};
}