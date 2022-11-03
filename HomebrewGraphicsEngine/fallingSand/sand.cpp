#include "sand.h"
#include "lava.h"


namespace Hogra::FallingSand {
	Sand::Sand() {
		float t = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		color = t * glm::vec4(0.886f, 0.8431f, 0.7764f, 1.0f) + (1.0f - t) * glm::vec4(0.6823f, 0.5607f, 0.3764f, 1.0f);
		density = 1.0f;
		meltingTemperature = 10000.0f;
		liquidConstructor = []() {
			return Allocator::New<Lava>();
		};
	}
}