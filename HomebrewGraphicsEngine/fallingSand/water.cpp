#include "water.h"
#include "steam.h"
#include "ice.h"

namespace Hogra::FallingSand {
	Water::Water() {
		color = { 0.0f, 0.4f, 1.0f, 0.4f };
		density = 1.0f;
		friction = 2.0f;
		boilingTemperature = 100.0f;
		freezingTemperature = 0.0f;
		gasConstructor = []() { return Allocator::New<FallingSand::Steam>(); };
		solidConstructor = []() { return Allocator::New<FallingSand::Ice>(); };
	}
}