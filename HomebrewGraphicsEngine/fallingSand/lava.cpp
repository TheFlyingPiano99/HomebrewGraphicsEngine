#include "lava.h"
#include "smoke.h"
#include "stone.h"


namespace Hogra::FallingSand {
	Lava::Lava() {
		color = { 25.0f, 0.5f, 0.5f, 1.0f };
		density = 1.0f;
		friction = 10.0f;
		temperature = 100000.0f;
		boilingTemperature = 1000000.0f;
		freezingTemperature = 1000.0f;
		gasConstructor = []() { return Allocator::New<FallingSand::Smoke>(); };
		solidConstructor = []() { return Allocator::New<FallingSand::Stone>(); };
	}

}