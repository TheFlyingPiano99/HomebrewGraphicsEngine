#pragma once
#include "solid.h"

namespace Hogra::FallingSand {

	class Sand : public Solid {
	public:
		Sand() {
			color = {0.9f, 0.9f, 0.05f, 1.0f};
			density = 1.0f;
		}

	private:

	};
}