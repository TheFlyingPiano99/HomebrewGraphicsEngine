#pragma once
#include "liquid.h"

namespace Hogra::FallingSand {

	class Water : public Liquid {
	public:
		Water() {
			color = { 0.0f, 0.0f, 1.0f, 1.0f };
			density = 1.0f;
			friction = 2.0f;
		}


	private:

	};
}