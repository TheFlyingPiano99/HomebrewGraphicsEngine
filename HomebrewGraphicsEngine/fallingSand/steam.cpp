#include "steam.h"
#include "water.h"

namespace Hogra::FallingSand {
	Steam::Steam() {
		color = { 0.9f, 0.91f, 1.0f, 0.3f };
		density = 0.2f;
		friction = 0.0f;
		condensationTemperature = 100.0f;
		liquidConstructor = []() {
			return Allocator::New<Water>(); 
		};
	};

}