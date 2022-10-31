#pragma once
#include "solid.h"

namespace Hogra::FallingSand {

	class Stone : public Solid {
	public:

		Stone() {
			{
				color = {0.6f, 0.6f, 0.6f, 1.0f};
				friction = 10.0f;
			}
		}

	private:

	};
}