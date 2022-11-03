#include "smoke.h"
#include "water.h"


namespace Hogra::FallingSand {
	Smoke::Smoke() {
		color = { 0.3f, 0.3f, 0.3f, 0.3f };
		density = 0.1f;
		friction = 0.0f;
		lifeTime = 10.0f;
		condensationTemperature = -10000.0f;
		liquidConstructor = []() {return Allocator::New<Water>(); };
	}

}