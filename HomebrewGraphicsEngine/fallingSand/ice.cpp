#include "ice.h"
#include "water.h"

namespace Hogra::FallingSand {
	Ice::Ice() {
		isStaticPosition = true;
		color = { 0.95f, 0.95f, 0.99f, 0.8f };
		friction = 1.0f;
		meltingTemperature = 0.0f;
		liquidConstructor = []() {
			return Allocator::New<Water>();
		};
	}
}