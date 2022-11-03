#include "stone.h"
#include "lava.h"


namespace Hogra::FallingSand {
	Stone::Stone() {
		{
			isStaticPosition = true;
			color = { 0.6f, 0.6f, 0.6f, 1.0f };
			friction = 10.0f;
			meltingTemperature = 10000.0f;
			liquidConstructor = []() {
				return Allocator::New<Lava>();
			};
		}
	}

}